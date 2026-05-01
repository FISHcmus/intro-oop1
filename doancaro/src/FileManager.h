#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Board.h"
#include <cstdint>
#include <string>

static const uint32_t SAVE_MAGIC = 0x4341524F;  // "CARO"
static const uint16_t SAVE_VERSION = 4;  // v4: extended gameMode (0=PvP, 1=PvAI Free, 2=PvAI Story) + story state
static const int MAX_SLOTS = 4;  // 0=autosave, 1-3=manual

enum class SaveGameMode : int { PvP = 0, PvAI = 1, Story = 2 };

inline const char* gameModeLabel(int gm) {
    static const char* k[] = {"PvP", "Free Play", "Story"};
    return (gm >= 0 && gm <= 2) ? k[gm] : "PvP";
}

inline const char* difficultyLabel(int aiDepth) {
    static const char* k[] = {"", "Easy", "Normal", "Hard"};
    return (aiDepth >= 1 && aiDepth <= 3) ? k[aiDepth] : "";
}

struct SaveHeader {
    uint32_t magic;
    uint16_t version;
    uint32_t checksum;
    int64_t  timestamp;
    float    playTime;
    int      moveCount;
    int      gameMode;        // 0=PvP, 1=PvAI Free Play, 2=PvAI Story Mode
    int      aiDepth;         // 1=Easy (greedy one-ply), 2=Normal (d=2), 3=Hard (d=3)
    int      currentTurn;     // 1=player1, 2=player2
    int      p1Wins;
    int      p2Wins;
    int      p1Moves;
    int      p2Moves;
    char     p1Name[32];
    char     p2Name[32];

    // v4 — Story Mode fields. Zero/-1 for non-Story saves.
    int      storySetId;          // 0..3 (Set1..FinalBoss); -1 if gameMode != 2
    int      storyMatchWins;      // Best-of-3 wins in current set
    int      storyMatchLosses;    // Best-of-3 losses in current set
    int      storyVoiCharges;     // -1 = locked
    int      storyGaCharges;      // -1 = locked
    int      storyNguaCharges;    // -1 = locked
    int      reserved[8];         // future-proofing; zero
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
