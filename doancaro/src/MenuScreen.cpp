#include "MenuScreen.h"
#include "raylib.h"

MenuScreen::MenuScreen() : selectedIndex(0), choice(MenuChoice::None) {
    items[0] = "New Game";
    items[1] = "Load Game";
    items[2] = "Settings";
    items[3] = "Exit";
}

void MenuScreen::update() {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        switch (selectedIndex) {
            case 0: choice = MenuChoice::NewGame; break;
            case 1: choice = MenuChoice::LoadGame; break;
            case 2: choice = MenuChoice::Settings; break;
            case 3: choice = MenuChoice::Exit; break;
        }
    }
}

void MenuScreen::draw() {
    int titleSize = 60;
    int itemSize = 30;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    const char* title = "CARO GAME";
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (screenW - titleWidth) / 2, screenH / 6, titleSize, WHITE);

    for (int i = 0; i < ITEM_COUNT; i++) {
        int textWidth = MeasureText(items[i], itemSize);
        int x = (screenW - textWidth) / 2;
        int y = screenH / 3 + i * 50;

        Color color = (i == selectedIndex) ? GOLD : LIGHTGRAY;
        DrawText(items[i], x, y, itemSize, color);

        if (i == selectedIndex) {
            DrawText(">", x - 30, y, itemSize, GOLD);
        }
    }

    DrawText("W/S or Arrow Keys to navigate, Enter to select",
             10, screenH - 30, 16, DARKGRAY);
}

void MenuScreen::reset() {
    selectedIndex = 0;
    choice = MenuChoice::None;
}
