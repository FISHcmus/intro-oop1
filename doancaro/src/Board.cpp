#include "Board.h"
#include <cmath>
#include <fstream>
#include <random>

// Zobrist static members
uint64_t Board::zobristTable[Board::SIZE][Board::SIZE][2] = {};
bool Board::zobristInitialized = false;

void Board::initZobrist() {
    if (zobristInitialized) return;
    std::mt19937_64 rng(0x12345678);  // fixed seed for determinism
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            for (int m = 0; m < 2; m++) {
                zobristTable[r][c][m] = rng();
            }
        }
    }
    zobristInitialized = true;
}

int Board::markIndex(CellState mark) {
    return (mark == CellState::PlayerX) ? 0 : 1;
}

Board::Board() : moveCount(0), lastMove({-1, -1}), zobristHash(0) {
    initZobrist();
    reset();
}

void Board::reset() {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            cells[r][c] = CellState::Empty;
        }
    }
    moveCount = 0;
    lastMove = {-1, -1};
    zobristHash = 0;
}

bool Board::placeMove(int row, int col, CellState mark) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) return false;
    if (cells[row][col] != CellState::Empty) return false;

    cells[row][col] = mark;
    zobristHash ^= zobristTable[row][col][markIndex(mark)];
    lastMove = {row, col};
    moveCount++;
    return true;
}

void Board::undoMove(int row, int col, Move previousLastMove) {
    zobristHash ^= zobristTable[row][col][markIndex(cells[row][col])];
    cells[row][col] = CellState::Empty;
    lastMove = previousLastMove;
    moveCount--;
}

CellState Board::getCell(int row, int col) const {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE)
        return CellState::Empty;
    return cells[row][col];
}

bool Board::isFull() const {
    return moveCount >= SIZE * SIZE;
}

bool Board::isEmpty(int row, int col) const {
    return getCell(row, col) == CellState::Empty;
}

int Board::countDirection(int row, int col, int dr, int dc, CellState mark) const {
    int count = 0;
    int r = row + dr;
    int c = col + dc;
    while (r >= 0 && r < SIZE && c >= 0 && c < SIZE && cells[r][c] == mark) {
        count++;
        r += dr;
        c += dc;
    }
    return count;
}

CellState Board::checkWinner(std::vector<Move>& winLine) const {
    // Check from last move position for efficiency
    if (lastMove.row < 0) return CellState::Empty;

    int row = lastMove.row;
    int col = lastMove.col;
    CellState mark = cells[row][col];
    if (mark == CellState::Empty) return CellState::Empty;

    // 4 directions: horizontal, vertical, diagonal-down, diagonal-up
    int dirs[][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (auto& d : dirs) {
        int countPos = countDirection(row, col, d[0], d[1], mark);
        int countNeg = countDirection(row, col, -d[0], -d[1], mark);
        int total = countPos + countNeg + 1;

        if (total >= 5) {
            winLine.clear();
            // Build the winning line from negative direction to positive
            for (int i = -countNeg; i <= countPos; i++) {
                int r = row + i * d[0];
                int c = col + i * d[1];
                winLine.push_back({r, c});
            }
            return mark;
        }
    }
    return CellState::Empty;
}

CellState Board::hasWinner() const {
    if (lastMove.row < 0) return CellState::Empty;

    int row = lastMove.row;
    int col = lastMove.col;
    CellState mark = cells[row][col];
    if (mark == CellState::Empty) return CellState::Empty;

    int dirs[][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    for (auto& d : dirs) {
        int total = countDirection(row, col, d[0], d[1], mark)
                  + countDirection(row, col, -d[0], -d[1], mark) + 1;
        if (total >= 5) return mark;
    }
    return CellState::Empty;
}

std::vector<Move> Board::getCandidateMoves(int radius) const {
    std::vector<Move> candidates;
    bool considered[SIZE][SIZE] = {};

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (cells[r][c] == CellState::Empty) continue;
            // Add empty cells within radius of this piece
            for (int dr = -radius; dr <= radius; dr++) {
                for (int dc = -radius; dc <= radius; dc++) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE
                        && cells[nr][nc] == CellState::Empty
                        && !considered[nr][nc]) {
                        considered[nr][nc] = true;
                        candidates.push_back({nr, nc});
                    }
                }
            }
        }
    }

    // If board is empty, suggest center
    if (candidates.empty() && moveCount == 0) {
        candidates.push_back({SIZE / 2, SIZE / 2});
    }
    return candidates;
}

bool Board::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << moveCount << "\n";
    file << lastMove.row << " " << lastMove.col << "\n";
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            file << static_cast<int>(cells[r][c]);
            if (c < SIZE - 1) file << " ";
        }
        file << "\n";
    }
    return file.good();
}

bool Board::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    file >> moveCount;
    file >> lastMove.row >> lastMove.col;
    zobristHash = 0;
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            int val;
            file >> val;
            cells[r][c] = static_cast<CellState>(val);
            if (cells[r][c] != CellState::Empty) {
                zobristHash ^= zobristTable[r][c][markIndex(cells[r][c])];
            }
        }
    }
    return file.good();
}
