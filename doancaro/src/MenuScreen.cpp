#include "MenuScreen.h"
#include "AudioManager.h"
#include "Fonts.h"
#include "Theme.h"
#include "UI.h"
#include "raylib.h"

#include <cstdio>
#include <vector>

static const float kBtnW       = 320.0f;
static const float kBtnH       = 56.0f;
static const float kBtnGap     = 12.0f;
static const float kFirstYFrac = 0.30f;
static const float kHintSize   = 14.0f;

// One-shot mapping from menu index to choice. Keep in sync with the items
// initializer in MenuScreen's constructor.
static const MenuChoice kChoiceFor[4] = {
    MenuChoice::NewGame,
    MenuChoice::LoadGame,
    MenuChoice::Settings,
    MenuChoice::Exit,
};

// Button anatomy: 4px outer frame, inner panel, plus a corner cap on each
// diagonal — a jade triangle in the top-left for Sơn (mountain), a cyan
// stacked-wave in the bottom-right for Thủy (water). Caps brighten and
// upsize on focus; pressed state offsets the whole stack +2px.
static const float kFrameInset = 4.0f;
static const float kCapRest    = 14.0f;
static const float kCapFocus   = 18.0f;
static const float kPressedDx  = 2.0f;

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

enum class BtnState { Rest, Focused, Pressed };

static void drawButton(Rectangle r, const char* label, BtnState state) {
    Color frameColor = Theme::palette.ink_sumi;
    Color bodyColor  = Theme::palette.slate_fog;
    Color textColor  = Theme::palette.son_bone;
    Color sonColor   = Theme::palette.son_jade;
    Color thuyColor  = Theme::palette.thuy_cyan;
    float capSize    = kCapRest;
    float ox         = 0.0f;
    float oy         = 0.0f;

    const bool awake = (state != BtnState::Rest);
    if (awake) {
        // Focused / Pressed share the gold framing — duality is "awake".
        frameColor = Theme::palette.gold_foil;
        bodyColor  = Theme::palette.ink_sumi;
        textColor  = Theme::palette.gold_foil;
        capSize    = kCapFocus;
        if (state == BtnState::Pressed) {
            ox = kPressedDx;
            oy = kPressedDx;
        }
    } else {
        sonColor  = Theme::withAlpha(sonColor,  140);
        thuyColor = Theme::withAlpha(thuyColor, 140);
    }

    Rectangle outer = { r.x + ox, r.y + oy, r.width, r.height };
    DrawRectangleRec(outer, frameColor);

    Rectangle inner = {
        outer.x + kFrameInset, outer.y + kFrameInset,
        outer.width  - 2.0f * kFrameInset,
        outer.height - 2.0f * kFrameInset
    };
    DrawRectangleRec(inner, bodyColor);

    const float ir = inner.x + inner.width;
    const float ib = inner.y + inner.height;

    // Sơn — single mountain triangle in the top-left corner of the panel.
    Vector2 sonV1 = { inner.x,           inner.y };
    Vector2 sonV2 = { inner.x,           inner.y + capSize };
    Vector2 sonV3 = { inner.x + capSize, inner.y };
    DrawTriangle(sonV1, sonV2, sonV3, sonColor);

    // Thủy — main wave triangle in the bottom-right corner.
    Vector2 thuyV1 = { ir,           ib };
    Vector2 thuyV2 = { ir,           ib - capSize };
    Vector2 thuyV3 = { ir - capSize, ib };
    DrawTriangle(thuyV1, thuyV2, thuyV3, thuyColor);

    // Stacked highlight curl, only when the cap is awake. Gives the wave
    // a layered feel rather than the symmetric look the mountain has.
    if (awake) {
        const float curlOff  = 6.0f;
        const float curlSize = capSize * 0.55f;
        Color curlColor = Theme::withAlpha(thuyColor, 200);
        Vector2 cV1 = { ir - curlOff,            ib - curlOff };
        Vector2 cV2 = { ir - curlOff,            ib - curlOff - curlSize };
        Vector2 cV3 = { ir - curlOff - curlSize, ib - curlOff };
        DrawTriangle(cV1, cV2, cV3, curlColor);
    }

    int tw = Fonts::measure(Fonts::bold, label, 22.0f);
    int tx = static_cast<int>(outer.x + (outer.width - static_cast<float>(tw)) * 0.5f);
    int ty = static_cast<int>(outer.y + (outer.height - 22.0f) * 0.5f);
    Fonts::draw(Fonts::bold, label, tx, ty, 22.0f, textColor);
}

MenuScreen::MenuScreen() : selectedIndex(0), choice(MenuChoice::None),
    items{"PLAY WITH AI", "PLAY VIA INTERNET", "SETTINGS", "EXIT"} {}

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
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedIndex = (selectedIndex - 1 + ITEM_COUNT) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedIndex = (selectedIndex + 1) % ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        audio.playMenuClickSound();
        choice = kChoiceFor[selectedIndex];
    }

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    Vector2 mouse = GetMousePosition();
    bool mouseMoved = UI::mouseMoved();

    for (int i = 0; i < ITEM_COUNT; i++) {
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
        BtnState st;
        if (i == selectedIndex) {
            // Pressed only when mouse is over AND held — keyboard nav stays
            // on Focused so the +2px offset doesn't shimmer on Up/Down.
            bool pressed = mouseDown && CheckCollisionPointRec(mouse, r);
            st = pressed ? BtnState::Pressed : BtnState::Focused;
        } else {
            st = BtnState::Rest;
        }
        drawButton(r, items[i], st);
    }

    const char* hint = "W/S or Arrow Keys to navigate, Enter to select";
    int hw = Fonts::measure(Fonts::body, hint, kHintSize);
    int hx = (w - hw) / 2;
    int hy = h - 28;
    Rectangle hp = { static_cast<float>(hx - 12),
                     static_cast<float>(hy - 4),
                     static_cast<float>(hw + 24),
                     22.0f };
    DrawRectangleRounded(hp, 0.5f, 4,
                         Theme::withAlpha(Theme::palette.ink_sumi, 160));
    Fonts::draw(Fonts::body, hint, hx, hy, kHintSize,
                Theme::withAlpha(Theme::palette.son_bone, 220));
}

void MenuScreen::reset() {
    selectedIndex = 0;
    choice = MenuChoice::None;
}
