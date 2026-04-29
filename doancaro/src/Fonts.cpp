#include "Fonts.h"

#include <vector>

namespace Fonts {

Font body  = {};
Font bold  = {};
Font title = {};
Font mono  = {};

static bool loaded = false;

// Latin codepoint set covering full Vietnamese (precomposed):
//   ASCII + Latin-1 Supplement + Latin Extended-A (ă đ) +
//   Latin Extended-B (Ơ ơ Ư ư) + Vietnamese Extended (tone-marked vowels) +
//   General Punctuation (— – ' ' " " …).
// Latin Extended-B is the block raylib's default (ASCII-only) load drops,
// which is why "người" / "nương" / "tưởng" render `?` for the ư/ơ glyphs.
static std::vector<int> buildLatinCodepoints() {
    std::vector<int> cps;
    auto addRange = [&](int lo, int hi) {
        for (int c = lo; c <= hi; ++c) cps.push_back(c);
    };
    addRange(0x0020, 0x007E);  // ASCII printable
    addRange(0x00A0, 0x00FF);  // Latin-1 Supplement
    addRange(0x0100, 0x017F);  // Latin Extended-A (ă đ)
    addRange(0x0180, 0x024F);  // Latin Extended-B (Ơ ơ Ư ư)
    addRange(0x1EA0, 0x1EF9);  // Vietnamese Extended (precomposed tone marks)
    addRange(0x2010, 0x203A);  // General Punctuation (em-dash, curly quotes)
    return cps;
}

// Mono adds the box-drawing block on top of the Latin set, used by the
// comic-panel border in Story Mode and any future ASCII-art HUD.
static std::vector<int> buildMonoCodepoints() {
    std::vector<int> cps = buildLatinCodepoints();
    for (int c = 0x2500; c <= 0x257F; ++c) cps.push_back(c);  // Box-drawing
    return cps;
}

void init() {
    std::vector<int> latinCps = buildLatinCodepoints();
    const int latinN = static_cast<int>(latinCps.size());

    // body / bold / title use Terminus (pixel font, full Vietnamese coverage)
    // for the retro/CRT aesthetic. mono stays JetBrains Mono — the storyline
    // comic panel renders large blocks of narration where vector readability
    // matters more than pixel character.
    body  = LoadFontEx("assets/fonts/Terminus-Regular.ttf", 32, latinCps.data(), latinN);
    bold  = LoadFontEx("assets/fonts/Terminus-Bold.ttf",    32, latinCps.data(), latinN);
    title = LoadFontEx("assets/fonts/Terminus-Bold.ttf",    48, latinCps.data(), latinN);

    std::vector<int> monoCps = buildMonoCodepoints();
    mono = LoadFontEx("assets/fonts/JetBrainsMono-Regular.ttf", 32,
                      monoCps.data(), static_cast<int>(monoCps.size()));

    SetTextureFilter(body.texture,  TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(bold.texture,  TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(title.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(mono.texture,  TEXTURE_FILTER_BILINEAR);
    loaded = true;
}

void cleanup() {
    if (!loaded) return;
    UnloadFont(body);
    UnloadFont(bold);
    UnloadFont(title);
    UnloadFont(mono);
    loaded = false;
}

int measure(Font font, const char* text, float fontSize, float spacing) {
    Vector2 v = MeasureTextEx(font, text, fontSize, spacing);
    return static_cast<int>(v.x);
}

void draw(Font font, const char* text, int x, int y, float fontSize, Color color,
          float spacing) {
    DrawTextEx(font, text, {static_cast<float>(x), static_cast<float>(y)},
               fontSize, spacing, color);
}

void drawCentered(Font font, const char* text, int cx, int y,
                  float fontSize, Color color, float spacing) {
    int w = measure(font, text, fontSize, spacing);
    draw(font, text, cx - w / 2, y, fontSize, color, spacing);
}

} // namespace Fonts
