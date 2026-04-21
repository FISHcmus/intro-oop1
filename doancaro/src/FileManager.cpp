#include "FileManager.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

std::string FileManager::getSaveDir() {
    std::string dir = "saves";
    struct stat st = {};
    if (stat(dir.c_str(), &st) != 0) {
        MKDIR(dir.c_str());
    }
    return dir;
}

std::string FileManager::getSlotPath(int slot) {
    if (slot == 0) return getSaveDir() + "/autosave.caro";
    char buf[64];
    std::snprintf(buf, sizeof(buf), "/slot%d.caro", slot);
    return getSaveDir() + buf;
}

uint32_t FileManager::computeCRC32(const void* data, size_t len) {
    const auto* bytes = static_cast<const uint8_t*>(data);
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (~(crc & 1) + 1));
        }
    }
    return ~crc;
}

bool FileManager::saveSlot(int slot, const SaveData& data) {
    // Prepare save data with checksum
    SaveData save = data;
    save.header.magic = SAVE_MAGIC;
    save.header.version = SAVE_VERSION;
    save.header.checksum = 0;
    save.header.checksum = computeCRC32(&save, sizeof(save));

    std::string path = getSlotPath(slot);
    std::string tmpPath = path + ".tmp";
    std::string bakPath = path + ".bak";

    // Write to temp file
    FILE* f = std::fopen(tmpPath.c_str(), "wb");
    if (!f) return false;

    size_t written = std::fwrite(&save, 1, sizeof(save), f);
    std::fclose(f);

    if (written != sizeof(save)) {
        std::remove(tmpPath.c_str());
        return false;
    }

    // Backup existing save
    struct stat st = {};
    if (stat(path.c_str(), &st) == 0) {
        std::rename(path.c_str(), bakPath.c_str());
    }

    // Atomic rename
    if (std::rename(tmpPath.c_str(), path.c_str()) != 0) {
        std::rename(bakPath.c_str(), path.c_str());  // restore
        return false;
    }

    return true;
}

LoadResult FileManager::loadSlot(int slot, SaveData& data) {
    std::string path = getSlotPath(slot);

    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return LoadResult::NotFound;

    size_t bytesRead = std::fread(&data, 1, sizeof(data), f);
    std::fclose(f);

    if (bytesRead != sizeof(data)) return LoadResult::Corrupt;
    if (data.header.magic != SAVE_MAGIC) return LoadResult::Corrupt;
    if (data.header.version > SAVE_VERSION) return LoadResult::VersionMismatch;

    // Validate checksum
    uint32_t storedCRC = data.header.checksum;
    data.header.checksum = 0;
    uint32_t computedCRC = computeCRC32(&data, sizeof(data));
    data.header.checksum = storedCRC;

    if (storedCRC != computedCRC) return LoadResult::Corrupt;

    // v2 used aiDepth {2=Easy, 3=Normal, 4=Hard}; v3 uses {1,2,3}.
    // Remap after checksum validation (CRC is over original bytes).
    if (data.header.version < SAVE_VERSION) {
        if (data.header.aiDepth >= 2 && data.header.aiDepth <= 4)
            data.header.aiDepth -= 1;
        else
            data.header.aiDepth = 3;
        data.header.version = SAVE_VERSION;
    }

    // Semantic validation
    if (data.header.currentTurn < 1 || data.header.currentTurn > 2) return LoadResult::Corrupt;
    for (auto& row : data.cells) {
        for (int cell : row) {
            if (cell < 0 || cell > 2) return LoadResult::Corrupt;
        }
    }

    return LoadResult::OK;
}

bool FileManager::loadSlotHeader(int slot, SaveHeader& header) {
    std::string path = getSlotPath(slot);

    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;

    size_t bytesRead = std::fread(&header, 1, sizeof(header), f);
    std::fclose(f);

    if (bytesRead != sizeof(header)) return false;
    return header.magic == SAVE_MAGIC;
}

bool FileManager::deleteSlot(int slot) {
    std::string path = getSlotPath(slot);
    return std::remove(path.c_str()) == 0;
}

bool FileManager::slotExists(int slot) {
    std::string path = getSlotPath(slot);
    struct stat st = {};
    return stat(path.c_str(), &st) == 0;
}
