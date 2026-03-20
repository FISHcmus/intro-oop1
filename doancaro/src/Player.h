#ifndef PLAYER_H
#define PLAYER_H

#include "Board.h"
#include <string>

class Player {
public:
    Player(const std::string& name, CellState mark);
    virtual ~Player() = default;

    // Returns the chosen move. For human players, this is set externally.
    // For AI, this computes the best move.
    virtual Move getMove(const Board& board);

    void setNextMove(Move move);

    const std::string& getName() const { return name; }
    CellState getMark() const { return mark; }
    int getWins() const { return wins; }
    int getMovesMade() const { return movesMade; }

    void addWin() { wins++; }
    void addMove() { movesMade++; }
    void resetStats();

protected:
    std::string name;
    CellState mark;
    int wins;
    int movesMade;
    Move nextMove;
};

#endif
