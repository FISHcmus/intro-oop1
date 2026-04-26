#include "SaveLoadScreen.h"
#include "AudioManager.h"
#include "Fonts.h"
#include "UI.h"
#include "raylib.h"
#include <cstdio>
#include <ctime>

SaveLoadScreen::SaveLoadScreen()
    : mode(SlotScreenMode::Save), result(SlotScreenResult::None),
      selectedSlot(1), deleteConfirmSlot(-1),
      headers{}, slotOccupied{} {}

void SaveLoadScreen::open(SlotScreenMode m) {
    mode = m;
    result = SlotScreenResult::None;
    selectedSlot = (m == SlotScreenMode::Save) ? 1 : 0;
    deleteConfirmSlot = -1;
    refreshSlotInfo();
}

void SaveLoadScreen::refreshSlotInfo() {
    for (int i = 0; i < MAX_SLOTS; i++) {
        slotOccupied[i] = FileManager::loadSlotHeader(i, headers[i]);
    }
}

bool SaveLoadScreen::canSelect(int slot) const {
    return (mode == SlotScreenMode::Save && slot >= 1)
        || (mode == SlotScreenMode::Load && slotOccupied[slot]);
}

void SaveLoadScreen::update(AudioManager& audio) {
    // Keyboard navigation
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        deleteConfirmSlot = -1;
        selectedSlot--;
        if (mode == SlotScreenMode::Save && selectedSlot < 1) selectedSlot = MAX_SLOTS - 1;
        else if (mode == SlotScreenMode::Load && selectedSlot < 0) selectedSlot = MAX_SLOTS - 1;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        deleteConfirmSlot = -1;
        selectedSlot++;
        if (selectedSlot >= MAX_SLOTS) {
            selectedSlot = (mode == SlotScreenMode::Save) ? 1 : 0;
        }
    }

    // Select slot
    if (IsKeyPressed(KEY_ENTER) && canSelect(selectedSlot)) {
        audio.playMenuClickSound();
        result = SlotScreenResult::Selected;
    }

    // Delete (Load mode only)
    if (mode == SlotScreenMode::Load && IsKeyPressed(KEY_DELETE)) {
        if (slotOccupied[selectedSlot] && selectedSlot > 0) {
            if (deleteConfirmSlot == selectedSlot) {
                FileManager::deleteSlot(selectedSlot);
                refreshSlotInfo();
                deleteConfirmSlot = -1;
            } else {
                deleteConfirmSlot = selectedSlot;
            }
        }
    }

    // Cancel
    if (IsKeyPressed(KEY_ESCAPE)) {
        audio.playMenuClickSound();
        result = SlotScreenResult::Cancelled;
    }

    // Mouse support
    int screenW = GetScreenWidth();
    int cardWidth = 500;
    int cardHeight = 80;
    int startX = (screenW - cardWidth) / 2;
    int startY = 120;
    int gap = 10;
    Vector2 mouse = GetMousePosition();
    bool mouseMoved = UI::mouseMoved();

    int firstSlot = (mode == SlotScreenMode::Save) ? 1 : 0;
    for (int i = firstSlot; i < MAX_SLOTS; i++) {
        int idx = (mode == SlotScreenMode::Save) ? (i - 1) : i;
        int y = startY + idx * (cardHeight + gap);
        Rectangle cardRect = {
            static_cast<float>(startX),
            static_cast<float>(y),
            static_cast<float>(cardWidth),
            static_cast<float>(cardHeight)
        };

        if (CheckCollisionPointRec(mouse, cardRect)) {
            if (mouseMoved) {
                selectedSlot = i;
                deleteConfirmSlot = -1;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && canSelect(i)) {
                audio.playMenuClickSound();
                result = SlotScreenResult::Selected;
            }
        }
    }
}

void SaveLoadScreen::draw() {
    int screenW = GetScreenWidth();

    // Title
    const char* title = (mode == SlotScreenMode::Save) ? "SAVE GAME" : "LOAD GAME";
    int titleWidth = Fonts::measure(Fonts::title, title, 50);
    Fonts::draw(Fonts::title, title, (screenW - titleWidth) / 2, 30, 50, WHITE);

    // Slot cards
    int cardWidth = 500;
    int cardHeight = 80;
    int startX = (screenW - cardWidth) / 2;
    int startY = 120;
    int gap = 10;

    int firstSlot = (mode == SlotScreenMode::Save) ? 1 : 0;
    for (int i = firstSlot; i < MAX_SLOTS; i++) {
        int idx = (mode == SlotScreenMode::Save) ? (i - 1) : i;
        int y = startY + idx * (cardHeight + gap);
        bool selected = (i == selectedSlot);
        drawSlotCard(i, startX, y, cardWidth, cardHeight, selected);
    }

    // Instructions
    int screenH = GetScreenHeight();
    if (mode == SlotScreenMode::Save) {
        Fonts::draw(Fonts::body, "[Enter] Save to slot  [ESC] Cancel",
                    10, screenH - 30, 16, DARKGRAY);
    } else {
        const char* hint = (deleteConfirmSlot >= 0)
            ? "[Del] Press again to confirm delete  [ESC] Cancel"
            : "[Enter] Load  [Del] Delete  [ESC] Cancel";
        Fonts::draw(Fonts::body, hint, 10, screenH - 30, 16, DARKGRAY);
    }
}

void SaveLoadScreen::drawSlotCard(int slot, int x, int y, int width, int height, bool selected) {
    // Background
    Color bgColor = selected ? Color{60, 60, 80, 255} : Color{40, 40, 50, 255};
    DrawRectangle(x, y, width, height, bgColor);

    // Border
    Color borderColor = selected ? GOLD : Color{80, 80, 80, 255};
    DrawRectangleLines(x, y, width, height, borderColor);

    // Delete confirmation highlight
    if (deleteConfirmSlot == slot) {
        DrawRectangleLines(x + 1, y + 1, width - 2, height - 2, RED);
    }

    // Slot label
    const char* slotLabel = (slot == 0) ? "[AUTO]" : nullptr;
    char slotBuf[16];
    if (slot > 0) {
        std::snprintf(slotBuf, sizeof(slotBuf), "[%d]", slot);
        slotLabel = slotBuf;
    }

    Fonts::draw(Fonts::bold, slotLabel, x + 10, y + 10, 20, selected ? GOLD : LIGHTGRAY);

    if (slotOccupied[slot]) {
        // Name
        const char* name = (slot == 0) ? "Autosave" : headers[slot].p1Name;
        Fonts::draw(Fonts::bold, name, x + 80, y + 10, 18, WHITE);

        // Date
        time_t t = static_cast<time_t>(headers[slot].timestamp);
        struct tm* tm = localtime(&t);
        char dateBuf[64];
        std::strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d %H:%M", tm);
        Fonts::draw(Fonts::body, dateBuf, x + 80, y + 32, 14, LIGHTGRAY);

        // Info line
        char infoBuf[128];
        int mins = static_cast<int>(headers[slot].playTime) / 60;
        int secs = static_cast<int>(headers[slot].playTime) % 60;
        const char* modeStr = (headers[slot].gameMode == 1) ? "PvAI" : "PvP";
        const char* diffStr = "";
        if (headers[slot].gameMode == 1) {
            switch (headers[slot].aiDepth) {
                case 1: diffStr = " Easy"; break;
                case 2: diffStr = " Normal"; break;
                case 3: diffStr = " Hard"; break;
                default: diffStr = ""; break;
            }
        }
        std::snprintf(infoBuf, sizeof(infoBuf), "%d moves  |  %02d:%02d  |  %s%s",
                      headers[slot].moveCount, mins, secs, modeStr, diffStr);
        Fonts::draw(Fonts::body, infoBuf, x + 80, y + 52, 14, DARKGRAY);

        // Delete confirm text
        if (deleteConfirmSlot == slot) {
            Fonts::draw(Fonts::bold, "Press Del again!", x + width - 140, y + 32, 14, RED);
        }
    } else {
        Fonts::draw(Fonts::body, "-- Empty --", x + 80, y + 30, 18, DARKGRAY);
    }
}
