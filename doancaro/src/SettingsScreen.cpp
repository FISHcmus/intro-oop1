#include "SettingsScreen.h"
#include "AudioManager.h"
#include "Theme.h"
#include "UIComponents.h"
#include "raylib.h"

#include <cstdio>

namespace {
constexpr float kRowW   = 480.0f;
constexpr float kRowH   = 56.0f;
constexpr float kRowGap = 12.0f;

Rectangle rowRect(int i, int screenW, int screenH) {
    float x = (static_cast<float>(screenW) - kRowW) * 0.5f;
    float y = static_cast<float>(screenH) / 3.0f
              + static_cast<float>(i) * (kRowH + kRowGap);
    return { x, y, kRowW, kRowH };
}
}  // namespace

SettingsScreen::SettingsScreen()
    : settings{true, false}, done(false) {}

void SettingsScreen::update(AudioManager& audio) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        audio.playMenuClickSound();
        done = true;
        return;
    }

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Vector2 mouse = GetMousePosition();

    // Row 1 — cheat unlock toggle.
    Rectangle cheatRect = rowRect(1, screenW, screenH);
    if (CheckCollisionPointRec(mouse, cheatRect)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        audio.playMenuClickSound();
        settings.cheatUnlockAll = !settings.cheatUnlockAll;
    }

    // Row 2 — back.
    Rectangle backRect = rowRect(2, screenW, screenH);
    if (CheckCollisionPointRec(mouse, backRect)
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        audio.playMenuClickSound();
        done = true;
    }
}

void SettingsScreen::draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangleGradientV(0, 0, screenW, screenH,
                           Theme::palette.bg_top,
                           Theme::palette.bg_bottom);

    UIC::drawTitle("SETTINGS", screenW, screenH);

    Vector2 mouse = GetMousePosition();
    bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    char modeBuf[64];
    char cheatBuf[96];
    std::snprintf(modeBuf, sizeof(modeBuf),
                  "Mode: < %s >", getGameModeLabel());
    std::snprintf(cheatBuf, sizeof(cheatBuf),
                  "Story cheat (mở khoá toàn bộ): < %s >",
                  settings.cheatUnlockAll ? "ON" : "OFF");
    const char* labels[ITEM_COUNT] = { modeBuf, cheatBuf, "BACK" };

    for (int i = 0; i < ITEM_COUNT; i++) {
        Rectangle r = rowRect(i, screenW, screenH);
        UIC::State st;
        if (i == 0) {
            st = UIC::State::Disabled;
        } else if (CheckCollisionPointRec(mouse, r)) {
            st = mouseDown ? UIC::State::Pressed : UIC::State::Focused;
        } else {
            st = UIC::State::Rest;
        }
        UIC::drawPrimaryButton(r, labels[i], st);
    }

    UIC::drawHintBar("Click vào dòng để chỉnh · Enter / ESC để quay lại",
                     screenW, screenH);
}

void SettingsScreen::reset() {
    done = false;
}

const char* SettingsScreen::getGameModeLabel() const {
    return settings.vsAI ? "Player vs AI" : "Player vs Player";
}
