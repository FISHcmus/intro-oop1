#ifndef OPENINGBOOK_H
#define OPENINGBOOK_H

#include "Board.h"
#include <cstddef>
#include <cstdint>
#include <unordered_map>

// Tier 1 hand-curated opening book for 15x15 free Gomoku.
// Keyed by Zobrist hash of stones on the board (order-independent).
// Each base entry is expanded under the D4 symmetry group (4 rotations
// x 2 reflections) so the book responds correctly from any orientation.
class OpeningBook {
public:
    OpeningBook();

    // Returns {true, move} if the hash is in the book, else {false, {-1,-1}}.
    // Avoids std::optional (C++17) to stay within the C++14 course constraint.
    struct Lookup {
        bool found;
        Move move;
    };
    Lookup query(uint64_t hash) const;

    std::size_t size() const { return table.size(); }

private:
    std::unordered_map<uint64_t, Move> table;
};

#endif
