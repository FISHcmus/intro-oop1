#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Player.h"
#include <cstdint>
#include <unordered_map>

// Transposition table entry
struct TTEntry {
    int depth;
    int score;
    int flag;  // 0 = exact, 1 = lower bound, 2 = upper bound
};

class AIPlayer : public Player {
public:
    AIPlayer(const std::string& name, CellState mark, int searchDepth = 4);

    Move getMove(const Board& board) override;

    void setSearchDepth(int depth) { searchDepth = depth; }
    int getSearchDepth() const { return searchDepth; }

private:
    int searchDepth;
    std::unordered_map<uint64_t, TTEntry> transTable;

    static constexpr int MAX_THREAT_DEPTH = 20;
    static constexpr int TIME_LIMIT_MEDIUM_MS = 1000;
    static constexpr int TIME_LIMIT_HARD_MS = 3000;

    // Pattern table: 3^5 = 243 entries, indexed by 5-cell window encoding
    // Cell encoding: 0=empty, 1=self, 2=opponent
    static int patternScore[243];
    static bool patternTableInit;
    static void initPatternTable();
    static int encodeWindow(const Board& board, int r, int c, int dr, int dc,
                            CellState selfMark);

    int minimax(Board& board, int depth, int alpha, int beta,
                bool maximizing, CellState aiMark, CellState opponentMark,
                int boardScore);
    Move iterativeDeepening(Board& searchBoard,
                            const std::vector<std::pair<int, Move>>& scored,
                            CellState aiMark, CellState opponentMark,
                            int timeLimitMs);
    static int scoreMove(Board& board, int row, int col, CellState moveMark,
                         CellState aiMark, CellState opponentMark);
    static int evaluate(const Board& board, CellState aiMark, CellState opponentMark);
    static int computeLocalScore(const Board& board, int row, int col,
                                 CellState aiMark, CellState opponentMark);

    // Threat-space search
    Move findThreatWin(Board& board, CellState attackMark, CellState defendMark, int depth);
    static int classifyThreat(const Board& board, int row, int col, CellState mark);
    static std::vector<Move> findThreats(Board& board, CellState mark);
    static std::vector<Move> findDefenses(Board& board, int row, int col, CellState attackMark);
};

#endif
