#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "OpeningBook.h"
#include "Player.h"
#include <cstdint>
#include <string>
#include <unordered_map>

enum class TTFlag : uint8_t { Exact, LowerBound, UpperBound };

struct TTEntry {
    int depth;
    int score;
    TTFlag flag;
    Move bestMove;  // {-1,-1} = not stored; used for move ordering
};

class AIPlayer : public Player {
public:
    AIPlayer(const std::string& name, CellState mark, int searchDepth = 3);
    ~AIPlayer() override;

    Move getMove(const Board& board) override;

    void setSearchDepth(int depth) { searchDepth = depth; }
    int getSearchDepth() const { return searchDepth; }

    // Debug info from last search
    struct DebugCandidate {
        Move move;
        int preScore;    // scoreMove ordering score
        int searchScore; // minimax result
    };
    struct DebugInfo {
        std::vector<DebugCandidate> topMoves;  // top 5 by search score
        Move chosenMove;
        int depthCompleted;
        int totalCandidates;
        long long searchTimeMs;
        std::string reason;  // "immediate_win", "greedy_one_ply", "minimax", "opening_book"
        // Transposition-table instrumentation (reset per getMove)
        long long nodesSearched;
        long long ttProbes;
        long long ttHits;          // find returned a non-end iterator
        long long ttCutoffs;       // hit had sufficient depth + flag/bound for early return
        long long ttStores;
        long long ttHoists;        // prior-best move actually swapped to front of scored
        int ttFinalSize;
    };
    DebugInfo lastDebug;
    const DebugInfo& getLastDebug() const { return lastDebug; }

private:
    int searchDepth;
    std::unordered_map<uint64_t, TTEntry> transTable;
    OpeningBook openingBook;

    static constexpr int MAX_THREAT_DEPTH = 20;

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
    static int scoreMove(Board& board, int row, int col, CellState moveMark,
                         CellState aiMark, CellState opponentMark);
    static int evaluate(const Board& board, CellState aiMark, CellState opponentMark);
    static int computeLocalScore(const Board& board, int row, int col,
                                 CellState aiMark, CellState opponentMark);

    // Threat-space search (currently unwired — kept for possible reuse)
    Move findThreatWin(Board& board, CellState attackMark, CellState defendMark, int depth);
    static int classifyThreat(const Board& board, int row, int col, CellState mark);
    static std::vector<Move> findThreats(Board& board, CellState mark);
    static std::vector<Move> findDefenses(Board& board, int row, int col, CellState attackMark);
};

#endif
