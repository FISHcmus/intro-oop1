#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Board.h"
#include <string>

struct GameSaveData {
    bool currentTurnIsPlayer1;
    int player1Wins;
    int player2Wins;
    int player1Moves;
    int player2Moves;
    std::string player1Name;
    std::string player2Name;
};

class FileManager {
public:
    static bool saveGame(const std::string& filename, const Board& board,
                         const GameSaveData& data);
    static bool loadGame(const std::string& filename, Board& board,
                         GameSaveData& data);
    static bool fileExists(const std::string& filename);
};

#endif
