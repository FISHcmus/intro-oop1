#include "AIPlayer.h"
#include <limits>
#include <algorithm>

AIPlayer::AIPlayer(const std::string& name, CellState mark, int searchDepth)
    : Player(name, mark), searchDepth(searchDepth) {}

Move AIPlayer::getMove(const Board& board) {
    // TODO: implement minimax + alpha-beta (task 98)
    // For now, pick the first candidate move
    auto candidates = board.getCandidateMoves();
    if (!candidates.empty()) {
        return candidates[0];
    }
    return {Board::SIZE / 2, Board::SIZE / 2};
}

int AIPlayer::minimax(Board& /*board*/, int /*depth*/, int /*alpha*/, int /*beta*/,
                      bool /*maximizing*/, CellState /*aiMark*/, CellState /*opponentMark*/) {
    // TODO: implement (task 98)
    return 0;
}

int AIPlayer::evaluate(const Board& /*board*/, CellState /*aiMark*/,
                       CellState /*opponentMark*/) const {
    // TODO: implement (task 98)
    return 0;
}

int AIPlayer::evaluateDirection(const Board& /*board*/, int /*row*/, int /*col*/,
                                int /*dr*/, int /*dc*/, CellState /*aiMark*/,
                                CellState /*opponentMark*/) const {
    // TODO: implement (task 98)
    return 0;
}
