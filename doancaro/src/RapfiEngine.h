#ifndef RAPFIENGINE_H
#define RAPFIENGINE_H

#include "Board.h"
#include <string>
#include <sys/types.h>

class RapfiEngine {
public:
    explicit RapfiEngine(const std::string& binaryDir, int timeoutMs = 10000);
    ~RapfiEngine();

    RapfiEngine(const RapfiEngine&) = delete;
    RapfiEngine& operator=(const RapfiEngine&) = delete;

    bool start();
    void stop();
    bool isRunning() const;

    Move sendTurn(const Move& opponentMove);
    Move sendBegin();
    Move sendBoard(const Board& board, CellState selfMark);

    long long getLastResponseTimeMs() const { return lastResponseTimeMs; }

private:
    std::string binaryDir;
    int timeoutMs;

    pid_t childPid;
    int pipeToChild;
    int pipeFromChild;

    bool running;
    bool initialized;

    long long lastResponseTimeMs;

    bool writeLine(const std::string& line);
    bool readMove(std::string& line, int timeoutMs);
    bool initialize();
    static Move parseResponse(const std::string& resp);
};

#endif
