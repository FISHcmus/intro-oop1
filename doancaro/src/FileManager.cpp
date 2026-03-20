#include "FileManager.h"
#include <fstream>

bool FileManager::saveGame(const std::string& filename, const Board& board,
                            const GameSaveData& data) {
    // Save board data
    if (!board.saveToFile(filename)) return false;

    // Append player data
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return false;

    file << (data.currentTurnIsPlayer1 ? 1 : 0) << "\n";
    file << data.player1Wins << " " << data.player2Wins << "\n";
    file << data.player1Moves << " " << data.player2Moves << "\n";
    file << data.player1Name << "\n";
    file << data.player2Name << "\n";
    return file.good();
}

bool FileManager::loadGame(const std::string& filename, Board& board,
                            GameSaveData& data) {
    if (!board.loadFromFile(filename)) return false;

    // Board::loadFromFile reads the first part; we need to read the rest
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    // Skip board data: moveCount + lastMove + SIZE*SIZE grid
    int skip;
    file >> skip >> skip >> skip; // moveCount, lastMove.row, lastMove.col
    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            file >> skip;
        }
    }

    int turnVal;
    file >> turnVal;
    data.currentTurnIsPlayer1 = (turnVal == 1);
    file >> data.player1Wins >> data.player2Wins;
    file >> data.player1Moves >> data.player2Moves;
    file >> data.player1Name;
    file >> data.player2Name;
    return file.good();
}

bool FileManager::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}
