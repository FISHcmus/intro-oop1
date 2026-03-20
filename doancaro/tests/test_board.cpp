#include <catch2/catch_test_macros.hpp>
#include "Board.h"
#include <cstdlib>
#include <string>

TEST_CASE("Board initializes empty", "[board]") {
    Board board;
    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            REQUIRE(board.getCell(r, c) == CellState::Empty);
        }
    }
    REQUIRE(board.getMoveCount() == 0);
    REQUIRE_FALSE(board.isFull());
}

TEST_CASE("Board place move", "[board]") {
    Board board;

    SECTION("valid placement") {
        REQUIRE(board.placeMove(0, 0, CellState::PlayerX));
        REQUIRE(board.getCell(0, 0) == CellState::PlayerX);
        REQUIRE(board.getMoveCount() == 1);
    }

    SECTION("cannot place on occupied cell") {
        board.placeMove(5, 5, CellState::PlayerX);
        REQUIRE_FALSE(board.placeMove(5, 5, CellState::PlayerO));
        REQUIRE(board.getCell(5, 5) == CellState::PlayerX);
    }

    SECTION("out of bounds rejected") {
        REQUIRE_FALSE(board.placeMove(-1, 0, CellState::PlayerX));
        REQUIRE_FALSE(board.placeMove(0, -1, CellState::PlayerX));
        REQUIRE_FALSE(board.placeMove(Board::SIZE, 0, CellState::PlayerX));
        REQUIRE_FALSE(board.placeMove(0, Board::SIZE, CellState::PlayerX));
    }
}

TEST_CASE("Board reset", "[board]") {
    Board board;
    board.placeMove(3, 3, CellState::PlayerX);
    board.placeMove(4, 4, CellState::PlayerO);
    board.reset();

    REQUIRE(board.getCell(3, 3) == CellState::Empty);
    REQUIRE(board.getCell(4, 4) == CellState::Empty);
    REQUIRE(board.getMoveCount() == 0);
}

TEST_CASE("Win detection — horizontal", "[board][win]") {
    Board board;
    std::vector<Move> winLine;

    for (int c = 0; c < 5; c++) {
        board.placeMove(3, c, CellState::PlayerX);
    }

    CellState winner = board.checkWinner(winLine);
    REQUIRE(winner == CellState::PlayerX);
    REQUIRE(winLine.size() == 5);
}

TEST_CASE("Win detection — vertical", "[board][win]") {
    Board board;
    std::vector<Move> winLine;

    for (int r = 2; r < 7; r++) {
        board.placeMove(r, 5, CellState::PlayerO);
    }

    CellState winner = board.checkWinner(winLine);
    REQUIRE(winner == CellState::PlayerO);
    REQUIRE(winLine.size() == 5);
}

TEST_CASE("Win detection — diagonal down-right", "[board][win]") {
    Board board;
    std::vector<Move> winLine;

    for (int i = 0; i < 5; i++) {
        board.placeMove(i, i, CellState::PlayerX);
    }

    CellState winner = board.checkWinner(winLine);
    REQUIRE(winner == CellState::PlayerX);
    REQUIRE(winLine.size() == 5);
}

TEST_CASE("Win detection — diagonal up-right", "[board][win]") {
    Board board;
    std::vector<Move> winLine;

    for (int i = 0; i < 5; i++) {
        board.placeMove(4 - i, i, CellState::PlayerO);
    }

    CellState winner = board.checkWinner(winLine);
    REQUIRE(winner == CellState::PlayerO);
    REQUIRE(winLine.size() == 5);
}

TEST_CASE("No winner with 4 in a row", "[board][win]") {
    Board board;
    std::vector<Move> winLine;

    for (int c = 0; c < 4; c++) {
        board.placeMove(0, c, CellState::PlayerX);
    }

    REQUIRE(board.checkWinner(winLine) == CellState::Empty);
}

TEST_CASE("Win with more than 5 in a row", "[board][win]") {
    Board board;
    std::vector<Move> winLine;

    for (int c = 0; c < 7; c++) {
        board.placeMove(5, c, CellState::PlayerX);
    }

    CellState winner = board.checkWinner(winLine);
    REQUIRE(winner == CellState::PlayerX);
    REQUIRE(winLine.size() >= 5);
}

TEST_CASE("Board isFull", "[board]") {
    Board board;

    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            CellState mark = ((r + c) % 2 == 0) ? CellState::PlayerX : CellState::PlayerO;
            board.placeMove(r, c, mark);
        }
    }

    REQUIRE(board.isFull());
}

TEST_CASE("Board isEmpty", "[board]") {
    Board board;
    REQUIRE(board.isEmpty(0, 0));
    board.placeMove(0, 0, CellState::PlayerX);
    REQUIRE_FALSE(board.isEmpty(0, 0));
}

TEST_CASE("getCandidateMoves — empty board returns center", "[board]") {
    Board board;
    auto candidates = board.getCandidateMoves();
    REQUIRE(candidates.size() == 1);
    REQUIRE(candidates[0].row == Board::SIZE / 2);
    REQUIRE(candidates[0].col == Board::SIZE / 2);
}

TEST_CASE("getCandidateMoves — returns nearby empty cells", "[board]") {
    Board board;
    board.placeMove(6, 6, CellState::PlayerX);

    auto candidates = board.getCandidateMoves(2);

    // Should not include (6,6) itself
    for (const auto& m : candidates) {
        REQUIRE_FALSE((m.row == 6 && m.col == 6));
    }

    // All candidates should be within radius 2 of (6,6)
    for (const auto& m : candidates) {
        REQUIRE(std::abs(m.row - 6) <= 2);
        REQUIRE(std::abs(m.col - 6) <= 2);
    }

    // Should have multiple candidates
    REQUIRE(candidates.size() > 1);
}

TEST_CASE("Board save and load", "[board][file]") {
    Board board;
    board.placeMove(0, 0, CellState::PlayerX);
    board.placeMove(1, 1, CellState::PlayerO);
    board.placeMove(2, 2, CellState::PlayerX);

    const char* tmpdir = std::getenv("TEMP");
    if (!tmpdir) tmpdir = std::getenv("TMPDIR");
    if (!tmpdir) tmpdir = "/tmp";
    std::string filename = std::string(tmpdir) + "/caro_test_board.dat";
    REQUIRE(board.saveToFile(filename));

    Board loaded;
    REQUIRE(loaded.loadFromFile(filename));

    REQUIRE(loaded.getCell(0, 0) == CellState::PlayerX);
    REQUIRE(loaded.getCell(1, 1) == CellState::PlayerO);
    REQUIRE(loaded.getCell(2, 2) == CellState::PlayerX);
    REQUIRE(loaded.getCell(3, 3) == CellState::Empty);
}
