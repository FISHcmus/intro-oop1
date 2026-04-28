// Standalone UI gallery — exercises every UIC component in every state.
// Build target: CaroGallery. Not bundled in CaroGame.
//
// Run: ./CaroGallery (from the build dir; assets are copied alongside).
//
// Layout: a sticky header (title) at top and a sticky hint bar at bottom.
// Everything between scrolls — mouse wheel, arrow keys, PageUp/Dn,
// Home/End. A thin gold thumb on the right edge tracks scroll position.

#include "Fonts.h"
#include "Theme.h"
#include "UIComponents.h"
#include "raylib.h"

namespace {

constexpr int kScreenW = 1100;
constexpr int kScreenH = 700;

constexpr int kHeaderH    = 90;
constexpr int kFooterH    = 36;
constexpr int kScrollbarW = 6;

constexpr float kSampleW   = 200.0f;
constexpr float kSampleH   = 56.0f;
constexpr float kSampleGap = 16.0f;

void drawSectionLabel(const char* label, int x, int y) {
    Fonts::draw(Fonts::bold, label, x, y, 18.0f,
                Theme::withAlpha(Theme::palette.son_bone, 220));
}

}  // namespace

int main() {
    InitWindow(kScreenW, kScreenH, "Caro UI Gallery");
    SetTargetFPS(60);
    Fonts::init();

    UIC::Speaker activeSpeaker = UIC::Speaker::CoSuTien;
    float scrollY = 0.0f;

    // Section anchors — absolute y inside the (unscrolled) scrollable region.
    // Total content height is the last section's bottom + breathing room;
    // we estimate generously so the scrollbar leaves a tiny dead zone past
    // the dialogue avatar instead of clipping it flush against the footer.
    const int btnY         = 130;
    const int liveY        = btnY + 100;          // 230
    const int titleSampleY = liveY + 110;         // 340
    const int comicY       = titleSampleY + 120;  // 460
    const int dialogueY    = comicY + 220;        // 680
    const int contentTotalH = dialogueY + 320;    // ~1000

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_ONE))   activeSpeaker = UIC::Speaker::CoSuTien;
        if (IsKeyPressed(KEY_TWO))   activeSpeaker = UIC::Speaker::SonTinh;
        if (IsKeyPressed(KEY_THREE)) activeSpeaker = UIC::Speaker::ThuyTinh;

        // --- Scroll input ---
        const float viewportH = static_cast<float>(kScreenH - kHeaderH - kFooterH);
        const float maxScroll = (contentTotalH > viewportH)
                                ? (static_cast<float>(contentTotalH) - viewportH)
                                : 0.0f;

        scrollY -= GetMouseWheelMove() * 40.0f;
        if (IsKeyDown(KEY_DOWN))         scrollY += 8.0f;
        if (IsKeyDown(KEY_UP))           scrollY -= 8.0f;
        if (IsKeyPressed(KEY_PAGE_DOWN)) scrollY += viewportH;
        if (IsKeyPressed(KEY_PAGE_UP))   scrollY -= viewportH;
        if (IsKeyPressed(KEY_HOME))      scrollY = 0.0f;
        if (IsKeyPressed(KEY_END))       scrollY = maxScroll;
        if (scrollY < 0.0f)        scrollY = 0.0f;
        if (scrollY > maxScroll)   scrollY = maxScroll;

        BeginDrawing();
        ClearBackground(Theme::palette.bg_bottom);
        DrawRectangleGradientV(0, 0, kScreenW, kScreenH,
                               Theme::palette.bg_top,
                               Theme::palette.bg_bottom);

        // --- Sticky header: title ---
        UIC::drawTitle("UI GALLERY", kScreenW, kScreenH, 20);
        DrawLine(0, kHeaderH, kScreenW, kHeaderH,
                 Theme::withAlpha(Theme::palette.gold_foil, 60));

        // --- Scrollable region (clipped) ---
        BeginScissorMode(0, kHeaderH, kScreenW, kScreenH - kHeaderH - kFooterH);

        const int sy = -static_cast<int>(scrollY);

        // drawPrimaryButton: every state side-by-side
        drawSectionLabel("drawPrimaryButton", 60, btnY - 28 + sy);
        const float btnX0 = (static_cast<float>(kScreenW) - (4 * kSampleW + 3 * kSampleGap)) * 0.5f;
        const char*      btnLabels[4] = { "REST", "FOCUSED", "PRESSED", "DISABLED" };
        const UIC::State btnStates[4] = { UIC::State::Rest, UIC::State::Focused,
                                          UIC::State::Pressed, UIC::State::Disabled };
        for (int i = 0; i < 4; ++i) {
            Rectangle r = { btnX0 + i * (kSampleW + kSampleGap),
                            static_cast<float>(btnY + sy), kSampleW, kSampleH };
            UIC::drawPrimaryButton(r, btnLabels[i], btnStates[i]);
        }

        // Interactive button — state driven by live mouse
        drawSectionLabel("Interactive (hover + click)", 60, liveY - 28 + sy);
        Rectangle rLive = { (static_cast<float>(kScreenW) - 320.0f) * 0.5f,
                            static_cast<float>(liveY + sy), 320.0f, kSampleH };
        Vector2 mouse = GetMousePosition();
        bool hover    = CheckCollisionPointRec(mouse, rLive);
        bool down     = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        UIC::State liveState =
            hover ? (down ? UIC::State::Pressed : UIC::State::Focused)
                  : UIC::State::Rest;
        UIC::drawPrimaryButton(rLive, "HOVER ME", liveState);

        // drawTitle sample at custom y
        drawSectionLabel("drawTitle (custom yTop)", 60, titleSampleY - 28 + sy);
        UIC::drawTitle("HELLO, SƠN TINH", kScreenW, kScreenH, titleSampleY + sy);

        // ComicPanel — narration
        drawSectionLabel("drawComicPanel — narration", 60, comicY - 28 + sy);
        UIC::ComicPanel cp = {
            "SET 2 — CUONG PHONG",
            "INTRO",
            "    .--.       .--.       .--.    \n"
            "   (    )     (    )     (    )   \n"
            "    `--'       `--'       `--'    \n"
            "    | ||       | ||       | ||    \n"
            "     \\\\\\(((( vortex ))))////    \n"
            "          (((((((((((             \n"
            "══════════════════════════════════\n"
            "      L  O  N  G    A  N          ",
            "Han dang ra suc tan cong Long Mach o Long An. Voi mot con loc "
            "xoay khong lo cung hang trieu binh linh den tu bien sau. Chung "
            "ta phai chan han bang moi gia.",
            50
        };
        UIC::drawComicPanel(cp, kScreenW / 2, comicY + sy);

        // DialoguePanel — speaker swappable via 1/2/3
        drawSectionLabel("drawDialoguePanel — press 1/2/3 to swap speaker",
                         60, dialogueY - 28 + sy);
        UIC::DialoguePanel dp = {
            activeSpeaker,
            "Son Tinh, hay cuoi Voi chin nga di nghenh chien.",
            40
        };
        UIC::drawDialoguePanel(dp, kScreenW / 2, dialogueY + sy);

        EndScissorMode();

        // --- Scrollbar thumb (right edge, between header and footer) ---
        if (maxScroll > 0.0f) {
            const float trackH = viewportH;
            float thumbH = trackH * trackH / static_cast<float>(contentTotalH);
            if (thumbH < 24.0f) thumbH = 24.0f;
            const float thumbY = static_cast<float>(kHeaderH) +
                                 (scrollY / maxScroll) * (trackH - thumbH);
            DrawRectangle(kScreenW - kScrollbarW - 4, static_cast<int>(thumbY),
                          kScrollbarW, static_cast<int>(thumbH),
                          Theme::withAlpha(Theme::palette.gold_foil, 200));
        }

        // --- Sticky footer: hint bar ---
        UIC::drawHintBar(
            "ESC quit · 1/2/3 speaker · wheel/↑↓/PgUp/PgDn/Home/End scroll",
            kScreenW, kScreenH);

        EndDrawing();
    }

    Fonts::cleanup();
    CloseWindow();
    return 0;
}
