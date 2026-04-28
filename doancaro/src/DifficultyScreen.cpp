#include "DifficultyScreen.h"
#include "AudioManager.h"
#include "Theme.h"
#include "UI.h"
#include "UIComponents.h"
#include "raylib.h"

namespace {
constexpr float kRowW   = 400.0f;
constexpr float kRowH   = 56.0f;
constexpr float kRowGap = 12.0f;

const char* const kLabels[3] = { "EASY", "NORMAL", "HARD" };
const int         kDepths[3] = { 1, 2, 3 };

Rectangle rowRect(int i, int screenW, int screenH) {
    float x = (static_cast<float>(screenW) - kRowW) * 0.5f;
    float y = static_cast<float>(screenH) / 3.0f
              + static_cast<float>(i) * (kRowH + kRowGap);
    return { x, y, kRowW, kRowH };
}
}  // namespace

DifficultyScreen::DifficultyScreen()
    : selectedIndex(0), done(false), cancelled(false), chosenDepth(3) {}

void DifficultyScreen::update(AudioManager& audio) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        audio.playMenuClickSound();
        if (selectedIndex < 3) {
            chosenDepth = kDepths[selectedIndex];
            done = true;
        } else {
            cancelled = true;
            done = true;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        audio.playMenuClickSound();
        cancelled = true;
        done = true;
    }

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Vector2 mouse = GetMousePosition();
    bool mouseMoved = UI::mouseMoved();

    for (int i = 0; i < ITEM_COUNT; i++) {
        Rectangle r = rowRect(i, screenW, screenH);
        if (CheckCollisionPointRec(mouse, r)) {
            if (mouseMoved) selectedIndex = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                audio.playMenuClickSound();
                if (i < 3) {
                    chosenDepth = kDepths[i];
                    done = true;
                } else {
                    cancelled = true;
                    done = true;
                }
            }
        }
    }
}

void DifficultyScreen::draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangleGradientV(0, 0, screenW, screenH,
                           Theme::palette.bg_top,
                           Theme::palette.bg_bottom);

    UIC::drawTitle("CHOOSE DIFFICULTY", screenW, screenH);

    Vector2 mouse = GetMousePosition();
    bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    const char* labels[ITEM_COUNT] = { kLabels[0], kLabels[1], kLabels[2], "CANCEL" };

    for (int i = 0; i < ITEM_COUNT; i++) {
        Rectangle r = rowRect(i, screenW, screenH);
        UIC::State st;
        if (i == selectedIndex) {
            bool pressed = mouseDown && CheckCollisionPointRec(mouse, r);
            st = pressed ? UIC::State::Pressed : UIC::State::Focused;
        } else {
            st = UIC::State::Rest;
        }
        UIC::drawPrimaryButton(r, labels[i], st);
    }

    UIC::drawHintBar("Up/Down to navigate, Enter to play, ESC to cancel",
                     screenW, screenH);
}

void DifficultyScreen::reset() {
    selectedIndex = 0;
    done = false;
    cancelled = false;
    chosenDepth = 3;
}
