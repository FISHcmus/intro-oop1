#pragma once
#include "raylib.h"

// Reusable UI primitives. Stateless free functions — each draw call is
// fully described by its arguments. State enum is the visual state
// driven by the call site's hover/click logic.
namespace UIC {

enum class State { Rest, Focused, Pressed, Disabled };

// Sơn/Thủy framed button: 4px sumi-ink frame, slate body, jade triangle in
// the top-left (Sơn / mountain), cyan stacked-wave in the bottom-right
// (Thủy / water). Focused/Pressed flip to gold framing; Pressed offsets
// the whole stack +2px. Disabled greys the body and dims the caps.
void drawPrimaryButton(Rectangle r, const char* label, State s);

// Centered headline. yTop < 0 uses screenH / 6 as a sane default.
void drawTitle(const char* label, int screenW, int screenH, int yTop = -1);

// Bottom-of-screen control hint. With backdrop = rounded sumi panel for
// readability over animated backgrounds; without = bare text on solid bg.
void drawHintBar(const char* hint, int screenW, int screenH,
                 bool withBackdrop = true);

// ---- Story panels — UTF-8 framed text rendered in JetBrains Mono ----

// Comic-book caption. Double-line frame ╔═╗║╠╣╚╝ with optional title bar
// (heading + right-anchored [TAG]). body is auto word-wrapped to widthCh.
struct ComicPanel {
    const char* title;     // nullptr → no title bar
    const char* tag;       // nullptr → no [TAG]
    const char* plot;      // nullptr → no illustration; else verbatim multi-line ASCII art
    const char* body;      // UTF-8; explicit '\n' preserved
    int         widthCh;   // total panel width in mono columns
};
void drawComicPanel(const ComicPanel& p, int xCenter, int yTop);

// Cowsay-style dialogue. Rounded bubble ╭─╮│╰╯ + tail + ASCII avatar,
// keyed by Speaker.
enum class Speaker { CoSuTien, SonTinh, ThuyTinh };
struct DialoguePanel {
    Speaker     speaker;
    const char* body;
    int         widthCh;   // bubble width in mono columns
};
void drawDialoguePanel(const DialoguePanel& p, int xCenter, int yTop);

} // namespace UIC
