#include "NetworkSession.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace {

#ifdef _WIN32
using NativeSocket = SOCKET;
const NativeSocket kInvalidNativeSocket = INVALID_SOCKET;
#else
using NativeSocket = int;
const NativeSocket kInvalidNativeSocket = -1;
#endif

NativeSocket toNative(long long fd) {
    return static_cast<NativeSocket>(fd);
}

long long toStored(NativeSocket fd) {
    return static_cast<long long>(fd);
}

CellState parseMark(const std::string& s) {
    if (s == "X") return CellState::PlayerX;
    if (s == "O") return CellState::PlayerO;
    return CellState::Empty;
}

bool parseInt(const std::string& s, int& out) {
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (!end || *end != '\0') return false;
    out = static_cast<int>(v);
    return true;
}

std::string joinTail(const std::vector<std::string>& fields, size_t start) {
    if (start >= fields.size()) return "";
    std::string out = fields[start];
    for (size_t i = start + 1; i < fields.size(); ++i) {
        out += '\t';
        out += fields[i];
    }
    return out;
}

}  // namespace

NetworkSession::NetworkSession()
    : mode(NetworkMode::None),
      running(false),
      socketFd(kInvalidSocket),
      listenFd(kInvalidSocket) {}

NetworkSession::~NetworkSession() {
    shutdown();
}

bool NetworkSession::beginSession(NetworkMode newMode) {
    shutdown();
    if (!initSockets()) return false;
    mode = newMode;
    running.store(true);
    return true;
}

bool NetworkSession::startLanHost(int port, const std::string& playerName) {
    if (!beginSession(NetworkMode::LanHost)) return false;
    selfName = playerName;
    worker = std::thread(&NetworkSession::runLanHost, this, port);
    return true;
}

bool NetworkSession::startLanJoin(const std::string& address, int port,
                                  const std::string& playerName) {
    if (!beginSession(NetworkMode::LanClient)) return false;
    selfName = playerName;
    worker = std::thread(&NetworkSession::runLanJoin, this, address, port);
    return true;
}

bool NetworkSession::startOnlineHost(const std::string& address, int port,
                                     const std::string& playerName) {
    if (!beginSession(NetworkMode::OnlineHost)) return false;
    selfName = playerName;
    worker = std::thread(&NetworkSession::runOnline, this, address, port, true,
                         std::string());
    return true;
}

bool NetworkSession::startOnlineJoin(const std::string& address, int port,
                                     const std::string& roomCode,
                                     const std::string& playerName) {
    if (!beginSession(NetworkMode::OnlineJoin)) return false;
    selfName = playerName;
    worker = std::thread(&NetworkSession::runOnline, this, address, port, false,
                         roomCode);
    return true;
}

void NetworkSession::shutdown() {
    running.store(false);
    closeSockets();
    if (worker.joinable()) worker.join();
    mode = NetworkMode::None;
}

void NetworkSession::sendMoveRequest(Move move) {
    char line[64];
    std::snprintf(line, sizeof(line), "MOVE\t%d\t%d", move.row, move.col);
    if (!sendLine(line)) {
        pushError("Failed to send move");
    }
}

void NetworkSession::sendAppliedMove(CellState mark, Move move) {
    const char* role = (mark == CellState::PlayerX) ? "X" : "O";
    char line[64];
    std::snprintf(line, sizeof(line), "APPLY\t%s\t%d\t%d",
                  role, move.row, move.col);
    if (!sendLine(line)) {
        pushError("Failed to sync move");
    }
}

std::vector<NetEvent> NetworkSession::pollEvents() {
    std::lock_guard<std::mutex> lock(eventMutex);
    std::vector<NetEvent> out(events.begin(), events.end());
    events.clear();
    return out;
}

void NetworkSession::pushEvent(NetEvent event) {
    std::lock_guard<std::mutex> lock(eventMutex);
    events.push_back(std::move(event));
}

void NetworkSession::pushError(const std::string& message) {
    NetEvent ev;
    ev.type = NetEventType::Error;
    ev.text = message;
    pushEvent(std::move(ev));
}

bool NetworkSession::sendLine(const std::string& line) {
    std::lock_guard<std::mutex> lock(sendMutex);
    if (!running.load() || socketFd == kInvalidSocket) return false;

    std::string payload = line;
    payload.push_back('\n');
    const char* data = payload.c_str();
    size_t total = 0;
    NativeSocket fd = toNative(socketFd);
    while (total < payload.size()) {
#ifdef _WIN32
        int sent = send(fd, data + total,
                        static_cast<int>(payload.size() - total), 0);
#else
        ssize_t sent = send(fd, data + total, payload.size() - total, 0);
#endif
        if (sent <= 0) return false;
        total += static_cast<size_t>(sent);
    }
    return true;
}

bool NetworkSession::recvLine(SocketHandle fd, std::string& line,
                              std::string& buffer) {
    NativeSocket sock = toNative(fd);
    while (true) {
        size_t newline = buffer.find('\n');
        if (newline != std::string::npos) {
            line = buffer.substr(0, newline);
            buffer.erase(0, newline + 1);
            if (!line.empty() && line.back() == '\r') line.pop_back();
            return true;
        }

        char chunk[512];
#ifdef _WIN32
        int received = recv(sock, chunk, sizeof(chunk), 0);
#else
        ssize_t received = recv(sock, chunk, sizeof(chunk), 0);
#endif
        if (received <= 0) return false;
        buffer.append(chunk, chunk + received);
    }
}

void NetworkSession::closeSockets() {
    closeSocket(socketFd);
    closeSocket(listenFd);
}

bool NetworkSession::initSockets() {
#ifdef _WIN32
    static std::mutex initMutex;
    static bool initialized = false;
    std::lock_guard<std::mutex> lock(initMutex);
    if (initialized) return true;
    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
    initialized = true;
#endif
    return true;
}

NetworkSession::SocketHandle NetworkSession::createListenSocket(int port) {
    char portBuf[16];
    std::snprintf(portBuf, sizeof(portBuf), "%d", port);

    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* result = nullptr;
    if (getaddrinfo(nullptr, portBuf, &hints, &result) != 0) return kInvalidSocket;

    NativeSocket listenSock = kInvalidNativeSocket;
    for (struct addrinfo* it = result; it != nullptr; it = it->ai_next) {
        listenSock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (listenSock == kInvalidNativeSocket) continue;

        int reuse = 1;
        setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<const char*>(&reuse), sizeof(reuse));

        if (bind(listenSock, it->ai_addr,
                 static_cast<int>(it->ai_addrlen)) == 0 &&
            listen(listenSock, 1) == 0) {
            break;
        }
        SocketHandle tmp = toStored(listenSock);
        closeSocket(tmp);
        listenSock = kInvalidNativeSocket;
    }

    freeaddrinfo(result);
    if (listenSock == kInvalidNativeSocket) return kInvalidSocket;
    return toStored(listenSock);
}

NetworkSession::SocketHandle NetworkSession::connectSocket(
    const std::string& address, int port) {
    char portBuf[16];
    std::snprintf(portBuf, sizeof(portBuf), "%d", port);

    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* result = nullptr;
    if (getaddrinfo(address.c_str(), portBuf, &hints, &result) != 0) {
        return kInvalidSocket;
    }

    NativeSocket sock = kInvalidNativeSocket;
    for (struct addrinfo* it = result; it != nullptr; it = it->ai_next) {
        sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (sock == kInvalidNativeSocket) continue;
        if (connect(sock, it->ai_addr, static_cast<int>(it->ai_addrlen)) == 0) {
            break;
        }
        SocketHandle tmp = toStored(sock);
        closeSocket(tmp);
        sock = kInvalidNativeSocket;
    }

    freeaddrinfo(result);
    if (sock == kInvalidNativeSocket) return kInvalidSocket;
    return toStored(sock);
}

std::string NetworkSession::guessLocalIPv4() const {
    NativeSocket sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == kInvalidNativeSocket) return "127.0.0.1";

    sockaddr_in remote{};
    remote.sin_family = AF_INET;
    remote.sin_port = htons(80);
    inet_pton(AF_INET, "8.8.8.8", &remote.sin_addr);
    connect(sock, reinterpret_cast<sockaddr*>(&remote), sizeof(remote));

    sockaddr_in local{};
#ifdef _WIN32
    int len = sizeof(local);
#else
    socklen_t len = sizeof(local);
#endif
    if (getsockname(sock, reinterpret_cast<sockaddr*>(&local), &len) != 0) {
        SocketHandle tmp = toStored(sock);
        closeSocket(tmp);
        return "127.0.0.1";
    }

    char ip[INET_ADDRSTRLEN] = "127.0.0.1";
    inet_ntop(AF_INET, &local.sin_addr, ip, sizeof(ip));
    {
        SocketHandle tmp = toStored(sock);
        closeSocket(tmp);
    }
    return ip;
}

void NetworkSession::closeSocket(SocketHandle& fd) {
    if (fd == kInvalidSocket) return;
    NativeSocket native = toNative(fd);
#ifdef _WIN32
    ::shutdown(native, SD_BOTH);
    closesocket(native);
#else
    ::shutdown(native, SHUT_RDWR);
    close(native);
#endif
    fd = kInvalidSocket;
}

std::vector<std::string> NetworkSession::splitFields(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    for (char ch : line) {
        if (ch == '\t') {
            fields.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }
    fields.push_back(current);
    return fields;
}

void NetworkSession::runLanHost(int port) {
    listenFd = createListenSocket(port);
    if (listenFd == kInvalidSocket) {
        pushError("Failed to host LAN game");
        running.store(false);
        return;
    }

    {
        NetEvent ev;
        ev.type = NetEventType::Waiting;
        ev.text = "Hosting on " + guessLocalIPv4() + ":" + std::to_string(port);
        pushEvent(std::move(ev));
    }

    sockaddr_in peerAddr{};
#ifdef _WIN32
    int peerLen = sizeof(peerAddr);
#else
    socklen_t peerLen = sizeof(peerAddr);
#endif
    NativeSocket accepted = accept(toNative(listenFd),
                                   reinterpret_cast<sockaddr*>(&peerAddr),
                                   &peerLen);
    if (!running.load()) return;
    if (accepted == kInvalidNativeSocket) {
        pushError("Failed to accept LAN client");
        running.store(false);
        return;
    }

    socketFd = toStored(accepted);
    closeSocket(listenFd);

    std::string buffer;
    std::string line;
    if (!recvLine(socketFd, line, buffer)) {
        pushError("LAN client disconnected during handshake");
        running.store(false);
        return;
    }

    std::vector<std::string> fields = splitFields(line);
    if (fields.size() < 2 || fields[0] != "HELLO") {
        pushError("Invalid LAN handshake");
        running.store(false);
        return;
    }

    const std::string peerName = fields[1];
    sendLine("START\tO\t" + selfName + "\t" + peerName);

    NetEvent start;
    start.type = NetEventType::MatchStarted;
    start.mark = CellState::PlayerX;
    start.player1Name = selfName;
    start.player2Name = peerName;
    pushEvent(std::move(start));

    while (running.load()) {
        if (!recvLine(socketFd, line, buffer)) break;
        fields = splitFields(line);
        if (fields.empty()) continue;

        if (fields[0] == "MOVE" && fields.size() >= 3) {
            int row = -1;
            int col = -1;
            if (!parseInt(fields[1], row) || !parseInt(fields[2], col)) {
                continue;
            }
            NetEvent move;
            move.type = NetEventType::MoveRequest;
            move.mark = CellState::PlayerO;
            move.move = {row, col};
            pushEvent(std::move(move));
        } else if (fields[0] == "QUIT") {
            break;
        }
    }

    if (running.load()) {
        NetEvent ev;
        ev.type = NetEventType::PeerDisconnected;
        ev.text = "LAN player disconnected";
        pushEvent(std::move(ev));
    }
    running.store(false);
}

void NetworkSession::runLanJoin(const std::string& address, int port) {
    socketFd = connectSocket(address, port);
    if (socketFd == kInvalidSocket) {
        pushError("Failed to connect to LAN host");
        running.store(false);
        return;
    }

    sendLine("HELLO\t" + selfName);

    std::string buffer;
    std::string line;
    while (running.load()) {
        if (!recvLine(socketFd, line, buffer)) break;
        std::vector<std::string> fields = splitFields(line);
        if (fields.empty()) continue;

        if (fields[0] == "START" && fields.size() >= 4) {
            NetEvent start;
            start.type = NetEventType::MatchStarted;
            start.mark = parseMark(fields[1]);
            start.player1Name = fields[2];
            start.player2Name = fields[3];
            pushEvent(std::move(start));
        } else if (fields[0] == "APPLY" && fields.size() >= 4) {
            int row = -1;
            int col = -1;
            if (!parseInt(fields[2], row) || !parseInt(fields[3], col)) continue;
            NetEvent move;
            move.type = NetEventType::MoveApplied;
            move.mark = parseMark(fields[1]);
            move.move = {row, col};
            pushEvent(std::move(move));
        } else if (fields[0] == "ERROR") {
            pushError(joinTail(fields, 1));
        } else if (fields[0] == "QUIT") {
            break;
        }
    }

    if (running.load()) {
        NetEvent ev;
        ev.type = NetEventType::PeerDisconnected;
        ev.text = "Connection to LAN host lost";
        pushEvent(std::move(ev));
    }
    running.store(false);
}

void NetworkSession::runOnline(const std::string& address, int port,
                               bool createRoom,
                               const std::string& roomCode) {
    socketFd = connectSocket(address, port);
    if (socketFd == kInvalidSocket) {
        pushError("Failed to reach online server");
        running.store(false);
        return;
    }

    if (createRoom) {
        sendLine("CREATE\t" + selfName);
    } else {
        sendLine("JOIN\t" + roomCode + "\t" + selfName);
    }

    std::string buffer;
    std::string line;
    while (running.load()) {
        if (!recvLine(socketFd, line, buffer)) break;
        std::vector<std::string> fields = splitFields(line);
        if (fields.empty()) continue;

        if (fields[0] == "ROOM" && fields.size() >= 2) {
            NetEvent ev;
            ev.type = NetEventType::RoomCode;
            ev.roomCode = fields[1];
            ev.text = "Room code: " + fields[1];
            pushEvent(std::move(ev));
        } else if (fields[0] == "WAIT" && fields.size() >= 2) {
            NetEvent ev;
            ev.type = NetEventType::Waiting;
            ev.text = joinTail(fields, 1);
            pushEvent(std::move(ev));
        } else if (fields[0] == "START" && fields.size() >= 4) {
            NetEvent start;
            start.type = NetEventType::MatchStarted;
            start.mark = parseMark(fields[1]);
            start.player1Name = fields[2];
            start.player2Name = fields[3];
            pushEvent(std::move(start));
        } else if (fields[0] == "APPLY" && fields.size() >= 4) {
            int row = -1;
            int col = -1;
            if (!parseInt(fields[2], row) || !parseInt(fields[3], col)) continue;
            NetEvent move;
            move.type = NetEventType::MoveApplied;
            move.mark = parseMark(fields[1]);
            move.move = {row, col};
            pushEvent(std::move(move));
        } else if (fields[0] == "INFO") {
            NetEvent ev;
            ev.type = NetEventType::Info;
            ev.text = joinTail(fields, 1);
            pushEvent(std::move(ev));
        } else if (fields[0] == "ERROR") {
            pushError(joinTail(fields, 1));
        } else if (fields[0] == "QUIT") {
            break;
        }
    }

    if (running.load()) {
        NetEvent ev;
        ev.type = NetEventType::PeerDisconnected;
        ev.text = "Online session disconnected";
        pushEvent(std::move(ev));
    }
    running.store(false);
}
