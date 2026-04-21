#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include "Board.h"
#include "OpeningBook.h"

TEST_CASE("OpeningBook contains entries", "[book]") {
    OpeningBook book;
    REQUIRE(book.size() > 0);
}

TEST_CASE("Book hits empty board", "[book]") {
    OpeningBook book;
    Board b;
    auto hit = book.query(b.getHash());
    REQUIRE(hit.found);
    // Empty board should steer us to the center.
    REQUIRE(hit.move.row == Board::SIZE / 2);
    REQUIRE(hit.move.col == Board::SIZE / 2);
}

TEST_CASE("Book responds to center-first opening", "[book]") {
    OpeningBook book;
    Board b;
    b.placeMove(7, 7, CellState::PlayerX);
    auto hit = book.query(b.getHash());
    REQUIRE(hit.found);
    // Standard reply is one diagonal step; symmetry may pick any of the
    // 4 equivalent corners. Assert it's a diagonal-adjacent empty cell.
    const int dr = hit.move.row - 7;
    const int dc = hit.move.col - 7;
    REQUIRE(std::abs(dr) == 1);
    REQUIRE(std::abs(dc) == 1);
}

TEST_CASE("Book takes center when opponent plays off-center", "[book]") {
    OpeningBook book;

    // 4 orthogonal-adjacent positions (d4-equivalent)
    const int orthoCoords[4][2] = {{7, 6}, {7, 8}, {6, 7}, {8, 7}};
    for (const auto& p : orthoCoords) {
        Board b;
        b.placeMove(p[0], p[1], CellState::PlayerX);
        auto hit = book.query(b.getHash());
        INFO("ortho-adj at (" << p[0] << "," << p[1] << ")");
        REQUIRE(hit.found);
        REQUIRE(hit.move.row == 7);
        REQUIRE(hit.move.col == 7);
    }

    // 4 diagonal-adjacent positions
    const int diagCoords[4][2] = {{6, 6}, {6, 8}, {8, 6}, {8, 8}};
    for (const auto& p : diagCoords) {
        Board b;
        b.placeMove(p[0], p[1], CellState::PlayerX);
        auto hit = book.query(b.getHash());
        INFO("diag-adj at (" << p[0] << "," << p[1] << ")");
        REQUIRE(hit.found);
        REQUIRE(hit.move.row == 7);
        REQUIRE(hit.move.col == 7);
    }
}

TEST_CASE("Book miss for position not in book", "[book]") {
    OpeningBook book;
    Board b;
    // A position with many stones — should not be in a Tier 1 book.
    b.placeMove(7, 7, CellState::PlayerX);
    b.placeMove(6, 6, CellState::PlayerO);
    b.placeMove(8, 8, CellState::PlayerX);
    b.placeMove(5, 5, CellState::PlayerO);
    b.placeMove(9, 9, CellState::PlayerX);
    auto hit = book.query(b.getHash());
    REQUIRE_FALSE(hit.found);
}
