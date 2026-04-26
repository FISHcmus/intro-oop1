#include "Theme.h"

namespace Theme {

const Palette palette = {
    /* son_jade          */ {110, 180, 130, 255},  // jade-cyan, faint green
    /* son_bone          */ {220, 210, 180, 255},  // ivory bone
    /* thuy_cyan         */ { 80, 200, 220, 255},  // wave cyan-teal
    /* thuy_pearl        */ {200, 230, 240, 255},  // pearl mist
    /* ink_sumi          */ { 20,  18,  25, 255},  // sumi black
    /* slate_fog         */ { 45,  52,  68, 255},  // slate panel fog
    /* paper_washi       */ {244, 238, 222, 255},  // washi paper
    /* gold_foil         */ {198, 162,  92, 255},  // antique gold (hairline)
    /* accent_vermillion */ {180,  70,  55, 255},  // CTA only
    /* bg_top            */ { 15,  18,  28, 255},  // ink-storm sky top
    /* bg_bottom         */ { 35,  40,  55, 255},  // ink-storm sky bottom
};

const Radius radius = { 6, 12, 20, 999 };

const Spacing spacing = { 4, 8, 12, 20, 32, 64 };

const Elevation elevation = {
    /* soft_shadow  */ {0, 0, 0, 22},  // ~8.6% black
    /* soft_blur_px */ 12,
};

const Motion motion = {
    /* ease_brush_ms     */ 280,
    /* ritual_bow_ms     */ 1200,
    /* ritual_stroke_ms  */ 800,
    /* ritual_close_ms   */ 600,
};

Type type = { nullptr, nullptr, nullptr };

static unsigned char clamp_u8(int v) {
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return static_cast<unsigned char>(v);
}

Color withAlpha(Color c, unsigned char a) {
    c.a = a;
    return c;
}

Color lerp(Color a, Color b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    const float r = static_cast<float>(a.r) + (static_cast<int>(b.r) - static_cast<int>(a.r)) * t;
    const float g = static_cast<float>(a.g) + (static_cast<int>(b.g) - static_cast<int>(a.g)) * t;
    const float bl = static_cast<float>(a.b) + (static_cast<int>(b.b) - static_cast<int>(a.b)) * t;
    const float al = static_cast<float>(a.a) + (static_cast<int>(b.a) - static_cast<int>(a.a)) * t;
    Color out;
    out.r = clamp_u8(static_cast<int>(r));
    out.g = clamp_u8(static_cast<int>(g));
    out.b = clamp_u8(static_cast<int>(bl));
    out.a = clamp_u8(static_cast<int>(al));
    return out;
}

} // namespace Theme
