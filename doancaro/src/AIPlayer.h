#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Player.h"

class AIPlayer : public Player {
public:
    AIPlayer(const std::string& name, CellState mark, int searchDepth = 4);

    Move getMove(const Board& board) override;

    void setSearchDepth(int depth) { searchDepth = depth; }
    int getSearchDepth() const { return searchDepth; }

private:
    int searchDepth;

    int minimax(Board& board, int depth, int alpha, int beta,
                bool maximizing, CellState aiMark, CellState opponentMark);
    int evaluate(const Board& board, CellState aiMark, CellState opponentMark) const;
    int evaluateDirection(const Board& board, int row, int col,
                          int dr, int dc, CellState aiMark, CellState opponentMark) const;
};

#endif
