#pragma once
#include "raylib.h"

// Wuxia-storm design tokens for Sơn Tinh / Thủy Tinh theme.
// Three adjectives drive every choice: cartoon (shapes), mythology
// (symbols), luxury (materials). Tokens belong to one of those layers.
//
// Usage:
//   Color c = Theme::palette.son_jade;
//   int r  = Theme::radius.md;
//   int ms = Theme::motion.ritual_stroke_ms;

namespace Theme {

struct Palette {
    // Sơn Tinh (núi) — mountain spirit
    Color son_jade;          // primary, jade-cyan glow
    Color son_bone;          // secondary, ivory stone

    // Thủy Tinh (nước) — water spirit
    Color thuy_cyan;         // primary, wave cyan-teal
    Color thuy_pearl;        // secondary, pearl mist

    // Ink + paper
    Color ink_sumi;          // text, line work, sumi black
    Color slate_fog;         // panel surface, slate fog
    Color paper_washi;       // bright surface, washi paper

    // Luxury accents (use sparingly)
    Color gold_foil;         // 1px hairline, rim of pieces
    Color accent_vermillion; // CTA only — NEVER as fill

    // Storm gradient (menu's mythological ink-storm backdrop)
    Color bg_top;            // top of ink-storm sky
    Color bg_bottom;         // bottom of ink-storm sky

    // Daylight scroll-painting sky (in-game scene, behind the 3D backdrop)
    Color sky_top;           // cream washi, scroll-paper white
    Color sky_mid;           // warm mist gray, painted-haze layer
    Color sky_horizon;       // cool fog gray, far mountains dissolve
};

struct Radius {
    int sm;     // 6  — ticks, tooltips
    int md;     // 12 — buttons, inputs
    int lg;     // 20 — panels, cards
    int pill;   // 999 — chips, hover rings
};

struct Spacing {
    int xs;     // 4
    int sm;     // 8
    int md;     // 12
    int lg;     // 20
    int xl;     // 32
    int xxl;    // 64
};

struct Elevation {
    Color soft_shadow;   // alpha-shaped drop shadow color
    int   soft_blur_px;  // gaussian-style fake blur radius
};

struct Motion {
    int ease_brush_ms;    // 280  — every UI transition
    int ritual_bow_ms;    // 1200 — match-open ink wash
    int ritual_stroke_ms; // 800  — winning-line brush stroke
    int ritual_close_ms;  // 600  — board fades like steam
};

// Font slots. Theme owns no fonts; the Fonts module fills these
// pointers after loading. Call sites refer to Theme::type.display_brush
// instead of Fonts::title etc., so swapping a font is a one-line change.
struct Type {
    const Font* display_brush;
    const Font* body_serif;
    const Font* mono;
};

extern const Palette   palette;
extern const Radius    radius;
extern const Spacing   spacing;
extern const Elevation elevation;
extern const Motion    motion;
extern Type            type;     // mutable: Fonts module fills it

// Helpers — keep tiny, no allocations.
Color withAlpha(Color c, unsigned char a);
Color lerp(Color a, Color b, float t);

} // namespace Theme
