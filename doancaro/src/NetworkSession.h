#pragma once

#include "Board.h"

#include <atomic>
#include <deque>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

enum class NetworkMode {
    None,
    LanHost,
    LanClient,
    OnlineHost,
    OnlineJoin,
};

enum class NetEventType {
    Info,
    Waiting,
    RoomCode,
    MatchStarted,
    MoveRequest,
    MoveApplied,
    PeerDisconnected,
    Error,
};

struct NetEvent {
    NetEventType type = NetEventType::Info;
    CellState    mark = CellState::Empty;
    Move         move{-1, -1};
    std::string  text;
    std::string  roomCode;
    std::string  player1Name;
    std::string  player2Name;
};

class NetworkSession {
public:
    NetworkSession();
    ~NetworkSession();

    bool startLanHost(int port, const std::string& playerName);
    bool startLanJoin(const std::string& address, int port,
                      const std::string& playerName);
    bool startOnlineHost(const std::string& address, int port,
                         const std::string& playerName);
    bool startOnlineJoin(const std::string& address, int port,
                         const std::string& roomCode,
                         const std::string& playerName);

    void shutdown();

    void sendMoveRequest(Move move);
    void sendAppliedMove(CellState mark, Move move);

    std::vector<NetEvent> pollEvents();

    bool isActive() const { return running.load(); }
    bool isNetworkMatch() const { return mode != NetworkMode::None; }
    bool localAppliesMoves() const { return mode == NetworkMode::LanHost; }
    NetworkMode getMode() const { return mode; }

private:
    using SocketHandle = long long;
    static const SocketHandle kInvalidSocket = -1;

    NetworkMode mode;
    std::atomic<bool> running;

    SocketHandle socketFd;
    SocketHandle listenFd;
    std::thread  worker;

    std::mutex            sendMutex;
    std::mutex            eventMutex;
    std::deque<NetEvent>  events;

    std::string selfName;

    bool beginSession(NetworkMode newMode);
    void runLanHost(int port);
    void runLanJoin(const std::string& address, int port);
    void runOnline(const std::string& address, int port,
                   bool createRoom, const std::string& roomCode);

    void pushEvent(NetEvent event);
    void pushError(const std::string& message);

    bool sendLine(const std::string& line);
    bool recvLine(SocketHandle fd, std::string& line, std::string& buffer);
    void closeSockets();

    bool initSockets();
    SocketHandle createListenSocket(int port);
    SocketHandle connectSocket(const std::string& address, int port);
    std::string guessLocalIPv4() const;
    static void closeSocket(SocketHandle& fd);
    static std::vector<std::string> splitFields(const std::string& line);
};
