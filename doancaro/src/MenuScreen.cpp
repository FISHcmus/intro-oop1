#include "MenuScreen.h"
#include "AudioManager.h"
#include "Fonts.h"
#include "Theme.h"
#include "UI.h"
#include "UIComponents.h"
#include "raylib.h"

#include <cstdio>
#include <vector>

static const float kBtnW       = 320.0f;
static const float kBtnH       = 56.0f;
static const float kBtnGap     = 12.0f;
static const float kFirstYFrac = 0.30f;

// One-shot mapping from menu index to choice. Keep in sync with the items
// initializer in MenuScreen's constructor. Multiplayer is intentionally
// unimplemented — it's rendered Disabled and excluded from selection.
static const MenuChoice kChoiceFor[6] = {
    MenuChoice::NewGame,
    MenuChoice::StoryMode,
    MenuChoice::LoadGame,
    MenuChoice::Multiplayer,
    MenuChoice::Settings,
    MenuChoice::Exit,
};

static bool isDisabled(int i) {
    return kChoiceFor[i] == MenuChoice::Multiplayer;
}

// Animated menu background: frame sequence baked from menu-background.mp4
// at build-time via ffmpeg. We pre-load every frame into VRAM so draw is
// just an index pick — no runtime decode, no disk I/O after preload.
// Falls back to the single still PNG if frames are missing, then a gradient.
static const float            kBgFrameFps     = 24.0f;
static const int              kBgMaxFrames    = 1024;  // sanity cap
static const char*            kBgFramePathFmt = "assets/images/menu-frames/frame_%03d.jpg";
static const char*            kBgStillPath    = "assets/images/menu-background-1.png";
static std::vector<Texture2D> s_menuFrames;
static Texture2D              s_menuStill   = {};
static bool                   s_menuBgTried = false;

static void ensureMenuBg() {
    if (s_menuBgTried) return;
    s_menuBgTried = true;

    s_menuFrames.reserve(128);
    for (int i = 1; i <= kBgMaxFrames; i++) {
        char path[64];
        std::snprintf(path, sizeof(path), kBgFramePathFmt, i);
        if (!FileExists(path)) break;
        Texture2D t = LoadTexture(path);
        if (t.id == 0) break;
        SetTextureFilter(t, TEXTURE_FILTER_BILINEAR);
        s_menuFrames.push_back(t);
    }
    if (!s_menuFrames.empty()) return;

    s_menuStill = LoadTexture(kBgStillPath);
    if (s_menuStill.id != 0) {
        SetTextureFilter(s_menuStill, TEXTURE_FILTER_BILINEAR);
    }
}

static Rectangle itemRect(int i, int screenW, int screenH) {
    float x = (static_cast<float>(screenW) - kBtnW) * 0.5f;
    float y = static_cast<float>(screenH) * kFirstYFrac
              + static_cast<float>(i) * (kBtnH + kBtnGap);
    return { x, y, kBtnW, kBtnH };
}

static void drawMenuBackground(int w, int h) {
    ensureMenuBg();

    Texture2D bg = {};
    if (!s_menuFrames.empty()) {
        const int n   = static_cast<int>(s_menuFrames.size());
        const int idx = static_cast<int>(GetTime() * kBgFrameFps) % n;
        bg = s_menuFrames[idx];
    } else if (s_menuStill.id != 0) {
        bg = s_menuStill;
    }
    if (bg.id == 0) {
        DrawRectangleGradientV(0, 0, w, h,
                               Theme::palette.bg_top,
                               Theme::palette.bg_bottom);
        return;
    }
    const auto wf = static_cast<float>(w);
    const auto hf = static_cast<float>(h);
    const auto iw = static_cast<float>(bg.width);
    const auto ih = static_cast<float>(bg.height);
    const float scale = (wf / iw) > (hf / ih) ? (wf / iw) : (hf / ih);
    const float dw = iw * scale;
    const float dh = ih * scale;
    Rectangle src = { 0.0f, 0.0f, iw, ih };
    Rectangle dst = { (wf - dw) * 0.5f, (hf - dh) * 0.5f, dw, dh };
    DrawTexturePro(bg, src, dst, {0, 0}, 0.0f, WHITE);
}

MenuScreen::MenuScreen() : selectedIndex(0), choice(MenuChoice::None),
    items{"PLAY WITH AI", "STORY MODE", "LOAD GAME",
          "PLAY VIA INTERNET", "SETTINGS", "EXIT"} {}

void MenuScreen::preload() {
    ensureMenuBg();  // file-scope static — no instance state needed
}

void MenuScreen::shutdown() {
    for (Texture2D& t : s_menuFrames) {
        if (t.id != 0) UnloadTexture(t);
    }
    s_menuFrames.clear();
    if (s_menuStill.id != 0) {
        UnloadTexture(s_menuStill);
        s_menuStill = {};
    }
    s_menuBgTried = false;
}

void MenuScreen::update(AudioManager& audio) {
    // Bounded skip-disabled loops: if every item somehow becomes disabled
    // we don't want to spin forever. ITEM_COUNT iterations is the worst
    // case for finding the next enabled item from any starting index.
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        for (int n = 0; n < ITEM_COUNT; ++n) {
            selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
            if (!isDisabled(selectedIndex)) break;
        }
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        for (int n = 0; n < ITEM_COUNT; ++n) {
            selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
            if (!isDisabled(selectedIndex)) break;
        }
    }
    if (IsKeyPressed(KEY_ENTER) && !isDisabled(selectedIndex)) {
        audio.playMenuClickSound();
        choice = kChoiceFor[selectedIndex];
    }

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Vector2 mouse = GetMousePosition();
    bool mouseMoved = UI::mouseMoved();

    for (int i = 0; i < ITEM_COUNT; i++) {
        if (isDisabled(i)) continue;
        Rectangle r = itemRect(i, screenW, screenH);
        if (CheckCollisionPointRec(mouse, r)) {
            if (mouseMoved) selectedIndex = i;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                audio.playMenuClickSound();
                choice = kChoiceFor[i];
            }
        }
    }
}

void MenuScreen::draw() {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    drawMenuBackground(w, h);

    Vector2 mouse = GetMousePosition();
    bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    for (int i = 0; i < ITEM_COUNT; i++) {
        Rectangle r = itemRect(i, w, h);
        UIC::State st;
        if (isDisabled(i)) {
            st = UIC::State::Disabled;
        } else if (i == selectedIndex) {
            // Pressed only when mouse is over AND held — keyboard nav stays
            // on Focused so the +2px offset doesn't shimmer on Up/Down.
            bool pressed = mouseDown && CheckCollisionPointRec(mouse, r);
            st = pressed ? UIC::State::Pressed : UIC::State::Focused;
        } else {
            st = UIC::State::Rest;
        }
        UIC::drawPrimaryButton(r, items[i], st);
    }

    UIC::drawHintBar("W/S or Arrow Keys to navigate, Enter to select", w, h);
}

void MenuScreen::reset() {
    selectedIndex = 0;
    choice = MenuChoice::None;
}
