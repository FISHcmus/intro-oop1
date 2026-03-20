#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>

// Cell state: 0 = empty, 1 = player 1 (X), 2 = player 2 (O)
enum class CellState { Empty = 0, PlayerX = 1, PlayerO = 2 };

struct Move {
    int row;
    int col;
};

class Board {
public:
    static const int SIZE = 12;

    Board();

    void reset();
    bool placeMove(int row, int col, CellState mark);
    CellState getCell(int row, int col) const;
    bool isFull() const;
    bool isEmpty(int row, int col) const;

    // Win detection: returns the winning CellState, or Empty if no winner.
    // If a winner is found, winLine is filled with the 5 winning positions.
    CellState checkWinner(std::vector<Move>& winLine) const;

    // Serialization
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

    // For AI: get all empty cells near existing pieces
    std::vector<Move> getCandidateMoves(int radius = 2) const;

    int getMoveCount() const { return moveCount; }
    Move getLastMove() const { return lastMove; }

private:
    CellState cells[SIZE][SIZE];
    int moveCount;
    Move lastMove;

    int countDirection(int row, int col, int dr, int dc, CellState mark) const;
};

#endif
