#include "OpeningBook.h"

#include <vector>

namespace {

// Apply one of 8 D4 transforms (4 rotations x 2 reflections) to (r,c)
// on a Board::SIZE x Board::SIZE grid.
void transformRC(int tid, int r, int c, int& outR, int& outC) {
    const int N = Board::SIZE;
    switch (tid) {
        case 0: outR = r;             outC = c;             break;  // identity
        case 1: outR = c;             outC = N - 1 - r;     break;  // rot90
        case 2: outR = N - 1 - r;     outC = N - 1 - c;     break;  // rot180
        case 3: outR = N - 1 - c;     outC = r;             break;  // rot270
        case 4: outR = r;             outC = N - 1 - c;     break;  // flip-h
        case 5: outR = N - 1 - r;     outC = c;             break;  // flip-v
        case 6: outR = c;             outC = r;             break;  // flip-main-diag
        case 7: outR = N - 1 - c;     outC = N - 1 - r;     break;  // flip-anti-diag
        default: outR = r;            outC = c;             break;
    }
}

struct Stone {
    int row;
    int col;
    CellState mark;
};

struct BaseEntry {
    std::vector<Stone> stones;
    Move bookMove;
};

}  // namespace

OpeningBook::OpeningBook() {
    const CellState X = CellState::PlayerX;

    // Tier 1: 6 canonical positions covering the opening moves 1-2.
    // Symmetry expansion multiplies these by up to 8.
    //
    // Rationale:
    //   - Empty board: always center. Controls 20 possible 5-in-a-row lines.
    //   - Opponent at center: one-step diagonal is the classical reply;
    //     stays close, keeps own lines open, contests the center.
    //   - Opponent anywhere else near center: take the center outright.
    const std::vector<BaseEntry> baseEntries = {
        {{},                 {Board::SIZE / 2, Board::SIZE / 2}},
        {{{7, 7, X}},        {6, 6}},
        {{{7, 6, X}},        {7, 7}},
        {{{6, 6, X}},        {7, 7}},
        {{{7, 5, X}},        {7, 7}},
        {{{6, 5, X}},        {7, 7}},
    };

    for (const auto& base : baseEntries) {
        for (int t = 0; t < 8; ++t) {
            Board b;
            for (const auto& s : base.stones) {
                int sr = 0;
                int sc = 0;
                transformRC(t, s.row, s.col, sr, sc);
                b.placeMove(sr, sc, s.mark);
            }
            int moveR = 0;
            int moveC = 0;
            transformRC(t, base.bookMove.row, base.bookMove.col, moveR, moveC);
            table[b.getHash()] = Move{moveR, moveC};
        }
    }
}

OpeningBook::Lookup OpeningBook::query(uint64_t hash) const {
    auto it = table.find(hash);
    if (it == table.end()) return Lookup{false, Move{-1, -1}};
    return Lookup{true, it->second};
}
