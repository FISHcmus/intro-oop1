#include "StorySigil.h"

#include "Fonts.h"
#include "Theme.h"

#include <cmath>

namespace StorySigil {

namespace {

constexpr float kPulseDuration   = 0.6f;
constexpr float kWashDuration    = 0.7f;
constexpr float kCaptionDuration = 1.5f;
constexpr float kCaptionFade     = 0.4f;

// Wuxia palette has no red — define a pair tuned to sit next to son_jade
// without screaming. Saturation kept moderate so it reads through the
// gold-foil ring without clobbering it.
const Color kWonGreen  = {110, 200, 130, 255};
const Color kWonGlow   = {180, 240, 200, 255};
const Color kLossRed   = {210,  60,  60, 255};
const Color kLossGlow  = {255, 110, 110, 255};

// Compute the three orb centers given the triangle layout. Layout convention:
// (centerX, bottomY) is the bottom-center of the equilateral frame; height
// = sideLen * sqrt(3)/2. Orbs sit INSIDE the frame, not on the vertices —
// matches the user's sketch (talisman-style "three jewels").
void orbPositions(const Layout& L, Vector2 out[3]) {
    const float h = static_cast<float>(L.sideLen) * 0.866f;
    out[0] = { static_cast<float>(L.centerX),
               static_cast<float>(L.bottomY) - h * 0.72f };  // top
    out[1] = { static_cast<float>(L.centerX) - L.sideLen * 0.21f,
               static_cast<float>(L.bottomY) - h * 0.22f };  // bottom-left
    out[2] = { static_cast<float>(L.centerX) + L.sideLen * 0.21f,
               static_cast<float>(L.bottomY) - h * 0.22f };  // bottom-right
}

void drawOrb(Vector2 pos, int r,
             StoryMode::OrbState state,
             bool pulsing, float pulsePhase) {
    using StoryMode::OrbState;

    if (state == OrbState::Pending) {
        // Hollow gold ring with a faint inner stroke for depth.
        DrawCircleLinesV(pos, static_cast<float>(r),
                         Theme::withAlpha(Theme::palette.gold_foil, 200));
        DrawCircleLinesV(pos, static_cast<float>(r) - 1.0f,
                         Theme::withAlpha(Theme::palette.gold_foil, 110));
        return;
    }

    Color core = (state == OrbState::Won) ? kWonGreen : kLossRed;
    Color glow = (state == OrbState::Won) ? kWonGlow  : kLossGlow;

    // Pulse modulates glow alpha for the orb that JUST filled.
    float glowAlphaScale = pulsing
        ? (0.45f + 0.55f * (0.5f + 0.5f * std::sin(pulsePhase * 6.28318f * 2.0f)))
        : 0.55f;
    auto glowA = static_cast<unsigned char>(160.0f * glowAlphaScale);
    auto haloA = static_cast<unsigned char>( 70.0f * glowAlphaScale);

    DrawCircleV(pos, static_cast<float>(r) + 5.0f, Theme::withAlpha(glow, haloA));
    DrawCircleV(pos, static_cast<float>(r) + 2.5f, Theme::withAlpha(glow, glowA));
    DrawCircleV(pos, static_cast<float>(r), core);
    DrawCircleLinesV(pos, static_cast<float>(r),
                     Theme::withAlpha(Theme::palette.gold_foil, 220));
}

}  // namespace

void draw(const Layout& L,
          const StoryMode::OrbState orbs[3],
          float currentTime,
          float lastFillTime) {
    using StoryMode::OrbState;

    const float h = static_cast<float>(L.sideLen) * 0.866f;
    Vector2 apex = { static_cast<float>(L.centerX),
                     static_cast<float>(L.bottomY) - h };
    Vector2 bl   = { static_cast<float>(L.centerX) - L.sideLen * 0.5f,
                     static_cast<float>(L.bottomY) };
    Vector2 br   = { static_cast<float>(L.centerX) + L.sideLen * 0.5f,
                     static_cast<float>(L.bottomY) };

    // Drop shadow under the frame — three offset thick lines, low alpha.
    Color shadow = Theme::withAlpha(Theme::palette.ink_sumi, 110);
    DrawLineEx({apex.x + 2.0f, apex.y + 2.0f},
               {bl.x   + 2.0f, bl.y   + 2.0f}, 4.0f, shadow);
    DrawLineEx({apex.x + 2.0f, apex.y + 2.0f},
               {br.x   + 2.0f, br.y   + 2.0f}, 4.0f, shadow);
    DrawLineEx({bl.x   + 2.0f, bl.y   + 2.0f},
               {br.x   + 2.0f, br.y   + 2.0f}, 4.0f, shadow);

    // Inner ink wash gives the orbs something to breathe against.
    DrawTriangle(apex, bl, br,
                 Theme::withAlpha(Theme::palette.ink_sumi, 160));

    // Gold-foil outline.
    Color line = Theme::palette.gold_foil;
    DrawLineEx(apex, bl, 2.5f, line);
    DrawLineEx(apex, br, 2.5f, line);
    DrawLineEx(bl,   br, 2.5f, line);

    // Identify which orb pulses — last-recorded non-Pending in chronological
    // order. The pulse is suppressed on first appearance (lastFillTime < 0).
    int lastIdx = -1;
    if (lastFillTime > 0.0f) {
        for (int i = 2; i >= 0; --i) {
            if (orbs[i] != OrbState::Pending) { lastIdx = i; break; }
        }
    }
    float pulseT = currentTime - lastFillTime;
    bool  pulsing = (lastFillTime > 0.0f) && (pulseT < kPulseDuration);
    float pulsePhase = pulseT / kPulseDuration;

    Vector2 orbs2D[3];
    orbPositions(L, orbs2D);
    for (int i = 0; i < 3; ++i) {
        drawOrb(orbs2D[i], L.orbRadius, orbs[i],
                pulsing && (i == lastIdx), pulsePhase);
    }
}

void drawScreenWash(int sigilX, int sigilY,
                    int screenW, int screenH,
                    Color washColor,
                    float fillTime,
                    float currentTime) {
    if (fillTime <= 0.0f) return;
    float t = currentTime - fillTime;
    if (t < 0.0f || t > kWashDuration) return;

    // Radius grows linearly to cover the diagonal; alpha decays linearly.
    float u = t / kWashDuration;
    float maxRadius = std::sqrt(static_cast<float>(screenW) * screenW +
                                static_cast<float>(screenH) * screenH);
    float r = u * maxRadius;
    auto innerA = static_cast<unsigned char>(110.0f * (1.0f - u));

    DrawCircleGradient(sigilX, sigilY, r,
                       Theme::withAlpha(washColor, innerA),
                       Theme::withAlpha(washColor, 0));
}

void drawCaption(const Layout& L,
                 const char* text,
                 float fillTime,
                 float currentTime) {
    if (!text || !*text || fillTime <= 0.0f) return;
    float t = currentTime - fillTime;
    if (t < 0.0f || t > kCaptionDuration) return;

    float fade = 1.0f;
    if (kCaptionDuration - t < kCaptionFade) {
        fade = (kCaptionDuration - t) / kCaptionFade;
    }

    int w = Fonts::measure(Fonts::body, text, 14);
    int x = L.centerX - w / 2;
    int y = L.bottomY + 6;
    Fonts::draw(Fonts::body, text, x, y, 14,
                Theme::withAlpha(Theme::palette.gold_foil,
                                 static_cast<unsigned char>(230.0f * fade)));
}

}  // namespace StorySigil
