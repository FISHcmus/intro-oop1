#include "Fonts.h"

#include <vector>

namespace Fonts {

Font body  = {};
Font bold  = {};
Font title = {};
Font mono  = {};

static bool loaded = false;

// Codepoint ranges used by mono-rendered panels: ASCII, Latin-1 Supplement,
// Latin Extended-A (ă đ ơ ư), Vietnamese Extended (tone-marked vowels), and
// the box-drawing block. Built once at init.
static std::vector<int> buildMonoCodepoints() {
    std::vector<int> cps;
    auto addRange = [&](int lo, int hi) {
        for (int c = lo; c <= hi; ++c) cps.push_back(c);
    };
    addRange(0x0020, 0x007E);  // ASCII printable
    addRange(0x00A0, 0x00FF);  // Latin-1 Supplement
    addRange(0x0100, 0x017F);  // Latin Extended-A
    addRange(0x1EA0, 0x1EF9);  // Vietnamese Extended (precomposed tone marks)
    addRange(0x2500, 0x257F);  // Box-drawing
    return cps;
}

void init() {
    body  = LoadFontEx("assets/fonts/Inter-Regular.ttf", 32, nullptr, 0);
    bold  = LoadFontEx("assets/fonts/Inter-Bold.ttf", 32, nullptr, 0);
    title = LoadFontEx("assets/fonts/BebasNeue.ttf", 48, nullptr, 0);

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

} // namespace Fonts
