#pragma once
#include "raylib.h"

// Global font system — call Fonts::init() after InitWindow(), Fonts::cleanup() before CloseWindow()
namespace Fonts {

extern Font body;      // Inter Regular — HUD, labels, small text
extern Font bold;      // Inter Bold — emphasis, player names
extern Font title;     // Bebas Neue — titles, headers, big text
extern Font mono;      // JetBrains Mono — ASCII frames, panels, code-like text
                       // Loaded with extended codepoints: ASCII + Latin-1 +
                       // Latin Extended-A + Vietnamese Extended (1EA0–1EF9)
                       // + box-drawing (2500–257F).

void init();
void cleanup();

// Helper: measure text width. spacing defaults to 1px between glyphs;
// pass 0 for mono panels where the font's advance already supplies the gap.
int measure(Font font, const char* text, float fontSize, float spacing = 1.0f);

// Helper: draw text with custom font. Same spacing convention as measure().
void draw(Font font, const char* text, int x, int y, float fontSize, Color color,
          float spacing = 1.0f);

} // namespace Fonts
