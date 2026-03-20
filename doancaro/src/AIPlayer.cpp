#include "AIPlayer.h"
#include <limits>
#include <algorithm>

AIPlayer::AIPlayer(const std::string& name, CellState mark, int searchDepth)
    : Player(name, mark), searchDepth(searchDepth) {}

Move AIPlayer::getMove(const Board& board) {
    auto candidates = board.getCandidateMoves();
    if (candidates.empty()) {
        return {Board::SIZE / 2, Board::SIZE / 2};
    }
    if (candidates.size() == 1) {
        return candidates[0];
    }

    CellState aiMark = mark;
    CellState opponentMark = (aiMark == CellState::PlayerX)
                                 ? CellState::PlayerO
                                 : CellState::PlayerX;

    int bestScore = -1000000;
    Move bestMove = candidates[0];

    for (const auto& move : candidates) {
        Board copy = board;
        copy.placeMove(move.row, move.col, aiMark);

        // Check for immediate win
        std::vector<Move> winLine;
        if (copy.checkWinner(winLine) == aiMark) {
            return move;
        }

        int score = minimax(copy, searchDepth - 1, -1000000, 1000000,
                            false, aiMark, opponentMark);
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    return bestMove;
}

int AIPlayer::minimax(Board& board, int depth, int alpha, int beta,
                      bool maximizing, CellState aiMark, CellState opponentMark) {
    std::vector<Move> winLine;
    CellState winner = board.checkWinner(winLine);
    if (winner == aiMark) return 100000 + depth;
    if (winner == opponentMark) return -100000 - depth;
    if (board.isFull()) return 0;
    if (depth <= 0) return evaluate(board, aiMark, opponentMark);

    auto candidates = board.getCandidateMoves();
    if (candidates.empty()) return evaluate(board, aiMark, opponentMark);

    if (maximizing) {
        int maxEval = -1000000;
        for (const auto& move : candidates) {
            Board copy = board;
            copy.placeMove(move.row, move.col, aiMark);
            int eval = minimax(copy, depth - 1, alpha, beta, false, aiMark, opponentMark);
            if (eval > maxEval) maxEval = eval;
            if (maxEval > alpha) alpha = maxEval;
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 1000000;
        for (const auto& move : candidates) {
            Board copy = board;
            copy.placeMove(move.row, move.col, opponentMark);
            int eval = minimax(copy, depth - 1, alpha, beta, true, aiMark, opponentMark);
            if (eval < minEval) minEval = eval;
            if (minEval < beta) beta = minEval;
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

int AIPlayer::evaluate(const Board& board, CellState aiMark,
                       CellState opponentMark) {
    int score = 0;
    int dirs[][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            if (board.getCell(r, c) == CellState::Empty) continue;
            for (auto& d : dirs) {
                score += evaluateDirection(board, r, c, d[0], d[1], aiMark, opponentMark);
            }
        }
    }
    return score;
}

int AIPlayer::evaluateDirection(const Board& board, int row, int col,
                                int dr, int dc, CellState aiMark,
                                CellState opponentMark) {
    // Count consecutive pieces from (row,col) in direction (dr,dc)
    CellState cell = board.getCell(row, col);
    if (cell == CellState::Empty) return 0;

    int count = 0;
    int openEnds = 0;
    int r = row;
    int c = col;

    // Count consecutive same-color pieces
    while (r >= 0 && r < Board::SIZE && c >= 0 && c < Board::SIZE
           && board.getCell(r, c) == cell) {
        count++;
        r += dr;
        c += dc;
    }

    // Check if end is open (empty)
    if (r >= 0 && r < Board::SIZE && c >= 0 && c < Board::SIZE
        && board.getCell(r, c) == CellState::Empty) {
        openEnds++;
    }

    // Check if start is open (one step before row,col)
    int sr = row - dr;
    int sc = col - dc;
    if (sr >= 0 && sr < Board::SIZE && sc >= 0 && sc < Board::SIZE
        && board.getCell(sr, sc) == CellState::Empty) {
        openEnds++;
    }

    // No open ends = blocked, worthless
    if (openEnds == 0) return 0;

    // Score based on count and open ends
    int score = 0;
    bool isAI = (cell == aiMark);

    if (count >= 5) {
        score = 100000;
    } else if (count == 4) {
        score = (openEnds == 2) ? 10000 : 1000;
    } else if (count == 3) {
        score = (openEnds == 2) ? 1000 : 100;
    } else if (count == 2) {
        score = (openEnds == 2) ? 100 : 10;
    } else {
        score = openEnds;
    }

    return isAI ? score : -score;
}
