#include "RapfiEngine.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>

RapfiEngine::RapfiEngine(const std::string& binaryDir, int timeoutMs)
    : binaryDir(binaryDir), timeoutMs(timeoutMs),
      childPid(-1), pipeToChild(-1), pipeFromChild(-1),
      running(false), initialized(false), lastResponseTimeMs(0) {}

RapfiEngine::~RapfiEngine() {
    stop();
}

bool RapfiEngine::start() {
    if (running) return true;

    int toChild[2];   // parent writes to toChild[1], child reads from toChild[0]
    int fromChild[2]; // child writes to fromChild[1], parent reads from fromChild[0]

    if (pipe(toChild) != 0 || pipe(fromChild) != 0) {
        return false;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(toChild[0]); close(toChild[1]);
        close(fromChild[0]); close(fromChild[1]);
        return false;
    }

    if (pid == 0) {
        // Child process
        close(toChild[1]);
        close(fromChild[0]);

        dup2(toChild[0], STDIN_FILENO);
        dup2(fromChild[1], STDOUT_FILENO);
        dup2(fromChild[1], STDERR_FILENO);

        close(toChild[0]);
        close(fromChild[1]);

        // chdir to binary directory so Rapfi finds config.toml and weights
        if (chdir(binaryDir.c_str()) != 0) {
            _exit(1);
        }

        execl("./rapfi", "rapfi", nullptr);
        _exit(1);
    }

    // Parent process
    close(toChild[0]);
    close(fromChild[1]);

    pipeToChild = toChild[1];
    pipeFromChild = fromChild[0];
    childPid = pid;
    running = true;

    if (!initialize()) {
        stop();
        return false;
    }

    initialized = true;
    return true;
}

void RapfiEngine::stop() {
    if (!running) return;

    writeLine("END");

    if (pipeToChild >= 0) { close(pipeToChild); pipeToChild = -1; }
    if (pipeFromChild >= 0) { close(pipeFromChild); pipeFromChild = -1; }

    if (childPid > 0) {
        int status;
        pid_t result = waitpid(childPid, &status, WNOHANG);
        if (result == 0) {
            // Still running, give it a moment
            usleep(100000); // 100ms
            result = waitpid(childPid, &status, WNOHANG);
            if (result == 0) {
                kill(childPid, SIGTERM);
                usleep(500000); // 500ms
                result = waitpid(childPid, &status, WNOHANG);
                if (result == 0) {
                    kill(childPid, SIGKILL);
                    waitpid(childPid, &status, 0);
                }
            }
        }
        childPid = -1;
    }

    running = false;
    initialized = false;
}

bool RapfiEngine::isRunning() const {
    if (!running || childPid <= 0) return false;
    int status;
    pid_t result = waitpid(childPid, &status, WNOHANG);
    return result == 0; // 0 means child still running
}

bool RapfiEngine::initialize() {
    // Send START command
    if (!writeLine("START 15")) return false;

    // Read response — expect "OK" (skip MESSAGE lines)
    std::string response;
    if (!readMove(response, 5000)) return false;
    // readMove skips MESSAGE/DEBUG lines, so response should be "OK"
    if (response.find("OK") == std::string::npos) return false;

    // Configure: standard rule (exactly 5), time control, pondering
    writeLine("INFO rule 1");
    writeLine("INFO timeout_turn " + std::to_string(timeoutMs));
    writeLine("INFO timeout_match 0");
    writeLine("INFO PONDERING 1");

    return true;
}

bool RapfiEngine::writeLine(const std::string& line) {
    if (pipeToChild < 0) return false;
    std::string data = line + "\n";
    ssize_t written = write(pipeToChild, data.c_str(), data.size());
    return written == static_cast<ssize_t>(data.size());
}

bool RapfiEngine::readMove(std::string& line, int timeoutMs) {
    if (pipeFromChild < 0) return false;

    line.clear();
    std::string buffer;
    char buf[256];

    auto deadline = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(timeoutMs);

    while (true) {
        auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now()).count();
        if (remaining <= 0) return false;

        struct pollfd pfd;
        pfd.fd = pipeFromChild;
        pfd.events = POLLIN;

        int ret = poll(&pfd, 1, static_cast<int>(remaining));
        if (ret <= 0) return false; // timeout or error

        ssize_t n = read(pipeFromChild, buf, sizeof(buf) - 1);
        if (n <= 0) return false; // pipe closed or error

        buf[n] = '\0';
        buffer += buf;

        // Process complete lines
        size_t pos;
        while ((pos = buffer.find('\n')) != std::string::npos) {
            std::string curLine = buffer.substr(0, pos);
            buffer = buffer.substr(pos + 1);

            // Strip \r
            if (!curLine.empty() && curLine.back() == '\r') {
                curLine.pop_back();
            }

            // Skip Rapfi debug/info output
            if (curLine.compare(0, 7, "MESSAGE") == 0) continue;
            if (curLine.compare(0, 5, "DEBUG") == 0) continue;
            if (curLine.compare(0, 5, "ERROR") == 0) continue;
            if (curLine.empty()) continue;

            // This is the actual response (coordinates or "OK")
            line = curLine;
            return true;
        }
    }
}

Move RapfiEngine::parseResponse(const std::string& resp) {
    // Format: "x,y" where x=col, y=row (Gomocup convention)
    size_t comma = resp.find(',');
    if (comma == std::string::npos) return {-1, -1};

    int x = std::atoi(resp.substr(0, comma).c_str());
    int y = std::atoi(resp.substr(comma + 1).c_str());

    // Convert Gomocup (x,y) to game (row,col)
    int row = y;
    int col = x;

    if (row < 0 || row >= Board::SIZE || col < 0 || col >= Board::SIZE) {
        return {-1, -1};
    }

    return {row, col};
}

Move RapfiEngine::sendBegin() {
    auto t0 = std::chrono::steady_clock::now();

    if (!writeLine("BEGIN")) return {-1, -1};

    std::string response;
    if (!readMove(response, timeoutMs + 2000)) return {-1, -1};

    auto t1 = std::chrono::steady_clock::now();
    lastResponseTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    return parseResponse(response);
}

Move RapfiEngine::sendTurn(const Move& opponentMove) {
    auto t0 = std::chrono::steady_clock::now();

    // Convert game (row,col) to Gomocup (x,y) = (col,row)
    std::string cmd = "TURN " + std::to_string(opponentMove.col) + "," +
                      std::to_string(opponentMove.row);
    if (!writeLine(cmd)) return {-1, -1};

    std::string response;
    if (!readMove(response, timeoutMs + 2000)) return {-1, -1};

    auto t1 = std::chrono::steady_clock::now();
    lastResponseTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    return parseResponse(response);
}

Move RapfiEngine::sendBoard(const Board& board, CellState selfMark) {
    auto t0 = std::chrono::steady_clock::now();

    if (!writeLine("BOARD")) return {-1, -1};

    // Send all non-empty cells
    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            CellState cell = board.getCell(r, c);
            if (cell == CellState::Empty) continue;

            int field = (cell == selfMark) ? 1 : 2;
            std::string line = std::to_string(c) + "," + std::to_string(r) + "," +
                               std::to_string(field);
            if (!writeLine(line)) return {-1, -1};
        }
    }

    if (!writeLine("DONE")) return {-1, -1};

    std::string response;
    if (!readMove(response, timeoutMs + 2000)) return {-1, -1};

    auto t1 = std::chrono::steady_clock::now();
    lastResponseTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    return parseResponse(response);
}
