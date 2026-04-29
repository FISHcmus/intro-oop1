#pragma once

#include "StoryMode.h"
#include "raylib.h"

// Story Mode "tam thai" sigil — a triangle frame with 3 orbs at its
// vertices. Each orb tracks one of the three best-of-3 matches in the
// current set: pending = hollow gold ring, won = jade-green filled,
// lost = ink-red filled. The most-recently-filled orb pulses for ~0.6s
// after a match concludes.
//
// Pure-draw module: no state of its own. Caller (Game) feeds the orb
// snapshot from StoryMode::State and the timestamp of the last fill so
// the pulse + screen-wash + caption stay in sync.

namespace StorySigil {

struct Layout {
    int centerX;        // bottom-center X of the triangle frame (in pixels)
    int bottomY;        // bottom-center Y of the triangle frame (in pixels)
    int sideLen   = 80; // triangle side length
    int orbRadius = 11;
};

// Draws the framed sigil with three orbs.
//   orbs           — chronological per-match outcome snapshot (size 3)
//   currentTime    — GetTime() this frame
//   lastFillTime   — GetTime() captured when the most recent orb was filled
//                    (pass < 0 to suppress the pulse; e.g. on first show)
void draw(const Layout& L,
          const StoryMode::OrbState orbs[3],
          float currentTime,
          float lastFillTime);

// Radial color burst from the sigil center. Alpha decays over ~0.7s.
// Call BEFORE HUD/text so labels stay readable through the wash.
//   washColor      — full-saturation hue for the burst (jade or red)
//   fillTime       — same timestamp passed to draw()
void drawScreenWash(int sigilX, int sigilY,
                    int screenW, int screenH,
                    Color washColor,
                    float fillTime,
                    float currentTime);

// Tiny "THẮNG TRẬN N" / "BẠI TRẬN N" caption under the sigil. Visible for
// ~1.5s with a 0.4s fade-out. Pass nullptr or empty to skip.
void drawCaption(const Layout& L,
                 const char* text,
                 float fillTime,
                 float currentTime);

}  // namespace StorySigil
