#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Board.h"
#include <cstdint>
#include <string>

static const uint32_t SAVE_MAGIC = 0x4341524F;  // "CARO"
static const uint16_t SAVE_VERSION = 3;  // v3: aiDepth encoding 1/2/3 (was 2/3/4 in v2)
static const int MAX_SLOTS = 4;  // 0=autosave, 1-3=manual

struct SaveHeader {
    uint32_t magic;
    uint16_t version;
    uint32_t checksum;
    int64_t  timestamp;
    float    playTime;
    int      moveCount;
    int      gameMode;        // 0=PvP, 1=PvAI
    int      aiDepth;         // 1=Easy (greedy one-ply), 2=Normal (d=2), 3=Hard (d=3)
    int      currentTurn;     // 1=player1, 2=player2
    int      p1Wins;
    int      p2Wins;
    int      p1Moves;
    int      p2Moves;
    char     p1Name[32];
    char     p2Name[32];
};

struct SaveData {
    SaveHeader header;
    int cells[Board::SIZE][Board::SIZE];
    int lastMoveRow;
    int lastMoveCol;
};

enum class LoadResult {
    OK,
    NotFound,
    Corrupt,
    VersionMismatch
};

class FileManager {
public:
    static std::string getSaveDir();
    static std::string getSlotPath(int slot);

    static bool saveSlot(int slot, const SaveData& data);
    static LoadResult loadSlot(int slot, SaveData& data);
    static bool loadSlotHeader(int slot, SaveHeader& header);
    static bool deleteSlot(int slot);
    static bool slotExists(int slot);

private:
    static uint32_t computeCRC32(const void* data, size_t len);
};

#endif
