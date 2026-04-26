#include "MenuScreen.h"
#include "AudioManager.h"
#include "Fonts.h"
#include "UI.h"
#include "raylib.h"

MenuScreen::MenuScreen() : selectedIndex(0), choice(MenuChoice::None),
    items{"New Game", "Load Game", "Settings", "Exit"} {}

void MenuScreen::update(AudioManager& audio) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        audio.playMenuClickSound();
        switch (selectedIndex) {
            case 0: choice = MenuChoice::NewGame; break;
            case 1: choice = MenuChoice::LoadGame; break;
            case 2: choice = MenuChoice::Settings; break;
            case 3: choice = MenuChoice::Exit; break;
        }
    }

    // Mouse support
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int itemSize = 30;
    int startY = screenH / 3;
    int itemHeight = 50;
    Vector2 mouse = GetMousePosition();
    bool mouseMoved = UI::mouseMoved();

    for (int i = 0; i < ITEM_COUNT; i++) {
        int textWidth = Fonts::measure(Fonts::bold, items[i], itemSize);
        int x = (screenW - textWidth) / 2;
        int y = startY + i * itemHeight;
        Rectangle itemRect = {
            static_cast<float>(x - 10),
            static_cast<float>(y - 5),
            static_cast<float>(textWidth + 40),
            static_cast<float>(itemHeight - 5)
        };

        if (CheckCollisionPointRec(mouse, itemRect)) {
            if (mouseMoved) selectedIndex = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                audio.playMenuClickSound();
                switch (i) {
                    case 0: choice = MenuChoice::NewGame; break;
                    case 1: choice = MenuChoice::LoadGame; break;
                    case 2: choice = MenuChoice::Settings; break;
                    case 3: choice = MenuChoice::Exit; break;
                    default: break;
                }
            }
        }
    }
}

void MenuScreen::draw() {
    int itemSize = 28;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    const char* title = "CARO GAME";
    int titleWidth = Fonts::measure(Fonts::title, title, 60);
    Fonts::draw(Fonts::title, title, (screenW - titleWidth) / 2, screenH / 6, 60, WHITE);

    for (int i = 0; i < ITEM_COUNT; i++) {
        int textWidth = Fonts::measure(Fonts::bold, items[i], itemSize);
        int x = (screenW - textWidth) / 2;
        int y = screenH / 3 + i * 50;

        Color color = (i == selectedIndex) ? GOLD : LIGHTGRAY;
        Fonts::draw(Fonts::bold, items[i], x, y, itemSize, color);

        if (i == selectedIndex) {
            Fonts::draw(Fonts::bold, ">", x - 30, y, itemSize, GOLD);
        }
    }

    Fonts::draw(Fonts::body, "W/S or Arrow Keys to navigate, Enter to select",
                10, screenH - 30, 16, DARKGRAY);
}

void MenuScreen::reset() {
    selectedIndex = 0;
    choice = MenuChoice::None;
}
