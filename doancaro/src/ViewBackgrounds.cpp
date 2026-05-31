#include "ViewBackgrounds.h"

#include "Theme.h"
#include "raylib.h"

#include <array>

namespace {

struct BackgroundSlot {
    const char* path;
    Color topWash;
    Color bottomWash;
    unsigned char shadowAlpha;
    unsigned char topWashAlpha;
    unsigned char bottomWashAlpha;
    Texture2D texture;
    bool tried;
};

std::array<BackgroundSlot, 3> s_slots = {{
    { "assets/images/view-backgrounds/settings-bg-wuxia-v1.png",
      Theme::palette.son_bone, Theme::palette.bg_bottom,
      68, 22, 108, {}, false },
    { "assets/images/view-backgrounds/multiplayer-bg-wuxia-v1.png",
      Theme::palette.thuy_cyan, Theme::palette.bg_bottom,
      74, 28, 118, {}, false },
    { "assets/images/view-backgrounds/saveload-bg-wuxia-v1.png",
      Theme::palette.gold_foil, Theme::palette.bg_bottom,
      66, 18, 104, {}, false },
}};

BackgroundSlot& slotFor(ViewBackgroundId id) {
    switch (id) {
        case ViewBackgroundId::Settings:    return s_slots[0];
        case ViewBackgroundId::Multiplayer: return s_slots[1];
        case ViewBackgroundId::SaveLoad:    return s_slots[2];
    }
    return s_slots[0];
}

void ensureLoaded(BackgroundSlot& slot) {
    if (slot.tried) return;
    slot.tried = true;

    if (!FileExists(slot.path)) return;

    slot.texture = LoadTexture(slot.path);
    if (slot.texture.id != 0) {
        SetTextureFilter(slot.texture, TEXTURE_FILTER_BILINEAR);
    }
}

void drawCover(const Texture2D& tex, int screenW, int screenH) {
    const float wf = static_cast<float>(screenW);
    const float hf = static_cast<float>(screenH);
    const float iw = static_cast<float>(tex.width);
    const float ih = static_cast<float>(tex.height);
    const float scale = (wf / iw) > (hf / ih) ? (wf / iw) : (hf / ih);
    const float dw = iw * scale;
    const float dh = ih * scale;

    Rectangle src = { 0.0f, 0.0f, iw, ih };
    Rectangle dst = { (wf - dw) * 0.5f, (hf - dh) * 0.5f, dw, dh };
    DrawTexturePro(tex, src, dst, {0.0f, 0.0f}, 0.0f, WHITE);
}

void drawOverlay(const BackgroundSlot& slot, int screenW, int screenH) {
    DrawRectangle(0, 0, screenW, screenH,
                  Theme::withAlpha(Theme::palette.ink_sumi, slot.shadowAlpha));
    DrawRectangleGradientV(0, 0, screenW, screenH,
                           Theme::withAlpha(slot.topWash, slot.topWashAlpha),
                           Theme::withAlpha(slot.bottomWash, slot.bottomWashAlpha));
}

}  // namespace

namespace ViewBackgrounds {

void preload() {
    for (BackgroundSlot& slot : s_slots) {
        ensureLoaded(slot);
    }
}

void draw(ViewBackgroundId id, int screenW, int screenH) {
    BackgroundSlot& slot = slotFor(id);
    ensureLoaded(slot);

    if (slot.texture.id != 0) {
        drawCover(slot.texture, screenW, screenH);
    } else {
        DrawRectangleGradientV(0, 0, screenW, screenH,
                               Theme::palette.bg_top,
                               Theme::palette.bg_bottom);
    }

    drawOverlay(slot, screenW, screenH);
}

void shutdown() {
    for (BackgroundSlot& slot : s_slots) {
        if (slot.texture.id != 0) {
            UnloadTexture(slot.texture);
            slot.texture = {};
        }
        slot.tried = false;
    }
}

}  // namespace ViewBackgrounds
