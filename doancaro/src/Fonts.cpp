#include "Fonts.h"

namespace Fonts {

Font body = {};
Font bold = {};
Font title = {};

static bool loaded = false;

void init() {
    body  = LoadFontEx("assets/fonts/Inter-Regular.ttf", 32, nullptr, 0);
    bold  = LoadFontEx("assets/fonts/Inter-Bold.ttf", 32, nullptr, 0);
    title = LoadFontEx("assets/fonts/BebasNeue.ttf", 48, nullptr, 0);
    SetTextureFilter(body.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(bold.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(title.texture, TEXTURE_FILTER_BILINEAR);
    loaded = true;
}

void cleanup() {
    if (!loaded) return;
    UnloadFont(body);
    UnloadFont(bold);
    UnloadFont(title);
    loaded = false;
}

int measure(Font font, const char* text, float fontSize) {
    Vector2 v = MeasureTextEx(font, text, fontSize, 1.0f);
    return static_cast<int>(v.x);
}

void draw(Font font, const char* text, int x, int y, float fontSize, Color color) {
    DrawTextEx(font, text, {static_cast<float>(x), static_cast<float>(y)},
               fontSize, 1.0f, color);
}

} // namespace Fonts
