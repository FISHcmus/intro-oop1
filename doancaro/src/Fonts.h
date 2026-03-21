#pragma once
#include "raylib.h"

// Global font system — call Fonts::init() after InitWindow(), Fonts::cleanup() before CloseWindow()
namespace Fonts {

extern Font body;      // Inter Regular — HUD, labels, small text
extern Font bold;      // Inter Bold — emphasis, player names
extern Font title;     // Bebas Neue — titles, headers, big text

void init();
void cleanup();

// Helper: measure text width for a given font and size
int measure(Font font, const char* text, float fontSize);

// Helper: draw text with custom font (spacing=1)
void draw(Font font, const char* text, int x, int y, float fontSize, Color color);

} // namespace Fonts
