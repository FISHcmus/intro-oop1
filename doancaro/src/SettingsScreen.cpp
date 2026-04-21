#include "SettingsScreen.h"
#include "Fonts.h"
#include "raylib.h"

SettingsScreen::SettingsScreen()
    : settings{true, 4}, selectedIndex(0), done(false) {}

void SettingsScreen::update() {
    // Keyboard navigation
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        switch (selectedIndex) {
            case 0:  // Toggle game mode
                settings.vsAI = !settings.vsAI;
                break;
            case 1:  // Cycle AI difficulty
                settings.aiDepth = (settings.aiDepth == 2) ? 4 : 2;
                break;
            case 2:  // Back
                done = true;
                break;
        }
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        switch (selectedIndex) {
            case 0:
                settings.vsAI = !settings.vsAI;
                break;
            case 1:
                settings.aiDepth = (settings.aiDepth == 2) ? 4 : 2;
                break;
            default:
                break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        done = true;
    }

    // Mouse support
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int startY = screenH / 3;
    int itemHeight = 50;
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < ITEM_COUNT; i++) {
        int y = startY + i * itemHeight;
        Rectangle itemRect = {
            static_cast<float>(screenW / 2 - 200),
            static_cast<float>(y - 5),
            400.0f,
            static_cast<float>(itemHeight - 5)
        };

        if (CheckCollisionPointRec(mouse, itemRect)) {
            selectedIndex = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                switch (i) {
                    case 0:
                        settings.vsAI = !settings.vsAI;
                        break;
                    case 1:
                        settings.aiDepth = (settings.aiDepth == 2) ? 4 : 2;
                        break;
                    case 2:
                        done = true;
                        break;
                }
            }
        }
    }
}

void SettingsScreen::draw() {
    int itemSize = 26;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Title
    const char* title = "SETTINGS";
    int titleWidth = Fonts::measure(Fonts::title, title, 50);
    Fonts::draw(Fonts::title, title, (screenW - titleWidth) / 2, screenH / 6, 50, WHITE);

    // Items
    int startY = screenH / 3;
    int itemHeight = 50;

    // Item 0: Game Mode
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "Game Mode: < %s >", getGameModeLabel());
        int textWidth = Fonts::measure(Fonts::bold, buf, itemSize);
        int x = (screenW - textWidth) / 2;
        int y = startY;
        Color color = (selectedIndex == 0) ? GOLD : LIGHTGRAY;
        Fonts::draw(Fonts::bold, buf, x, y, itemSize, color);
        if (selectedIndex == 0) Fonts::draw(Fonts::bold, ">", x - 30, y, itemSize, GOLD);
    }

    // Item 1: AI Difficulty
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "AI Difficulty: < %s >", getDifficultyLabel());
        int textWidth = Fonts::measure(Fonts::bold, buf, itemSize);
        int x = (screenW - textWidth) / 2;
        int y = startY + itemHeight;
        Color color;
        if (!settings.vsAI) {
            color = DARKGRAY;  // Greyed out when PvP
        } else {
            color = (selectedIndex == 1) ? GOLD : LIGHTGRAY;
        }
        Fonts::draw(Fonts::bold, buf, x, y, itemSize, color);
        if (selectedIndex == 1 && settings.vsAI) Fonts::draw(Fonts::bold, ">", x - 30, y, itemSize, GOLD);
    }

    // Item 2: Back
    {
        const char* label = "Back";
        int textWidth = Fonts::measure(Fonts::bold, label, itemSize);
        int x = (screenW - textWidth) / 2;
        int y = startY + 2 * itemHeight;
        Color color = (selectedIndex == 2) ? GOLD : LIGHTGRAY;
        Fonts::draw(Fonts::bold, label, x, y, itemSize, color);
        if (selectedIndex == 2) Fonts::draw(Fonts::bold, ">", x - 30, y, itemSize, GOLD);
    }

    // Instructions
    Fonts::draw(Fonts::body, "Left/Right to change, Enter to select, ESC to go back",
                10, screenH - 30, 16, DARKGRAY);
}

void SettingsScreen::reset() {
    selectedIndex = 0;
    done = false;
}

const char* SettingsScreen::getDifficultyLabel() const {
    return (settings.aiDepth == 2) ? "Easy" : "Hard";
}

const char* SettingsScreen::getGameModeLabel() const {
    return settings.vsAI ? "Player vs AI" : "Player vs Player";
}
