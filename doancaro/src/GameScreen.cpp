#include "GameScreen.h"
#include "Fonts.h"
#include "Theme.h"
#include "raylib.h"
#include <cstdio>

GameScreen::GameScreen() : hudX(0), hudY(0) {}

void GameScreen::drawHUD(const Player& player1, const Player& player2,
                          bool isPlayer1Turn, int totalMoves) {
    int screenW = GetScreenWidth();

    static constexpr int PANEL_W   = 200;
    static constexpr int PANEL_PAD = 12;
    int panelX = screenW - PANEL_W - 10;
    int panelY = 50;
    int panelH = 190;

    hudX = panelX + PANEL_PAD;
    hudY = panelY + PANEL_PAD;

    // Wuxia talisman panel: ink-sumi outer rim, slate-fog body, gold hairline,
    // jade rune top-left for Sơn, cyan rune bottom-right for Thủy.
    Rectangle outer = {
        static_cast<float>(panelX), static_cast<float>(panelY),
        static_cast<float>(PANEL_W), static_cast<float>(panelH)
    };
    DrawRectangleRounded(outer, 0.08f, 6, Theme::palette.ink_sumi);

    const float bodyInset = 3.0f;
    Rectangle body = {
        outer.x + bodyInset, outer.y + bodyInset,
        outer.width  - 2.0f * bodyInset,
        outer.height - 2.0f * bodyInset
    };
    DrawRectangleRounded(body, 0.08f, 6,
                         Theme::withAlpha(Theme::palette.slate_fog, 220));
    DrawRectangleRoundedLinesEx(body, 0.08f, 6, 1.0f, Theme::palette.gold_foil);

    // Corner runes — same Sơn/Thủy idiom as the menu, scaled small.
    const float runeSize = 8.0f;
    DrawTriangle(
        { body.x,            body.y },
        { body.x,            body.y + runeSize },
        { body.x + runeSize, body.y },
        Theme::palette.son_jade);
    const float br_x = body.x + body.width;
    const float br_y = body.y + body.height;
    DrawTriangle(
        { br_x,            br_y },
        { br_x,            br_y - runeSize },
        { br_x - runeSize, br_y },
        Theme::palette.thuy_cyan);

    // P1 = X = Sơn (jade), P2 = O = Thủy (cyan), active turn = gold.
    Color p1Color = isPlayer1Turn ? Theme::palette.gold_foil : Theme::palette.son_jade;
    Color p2Color = !isPlayer1Turn ? Theme::palette.gold_foil : Theme::palette.thuy_cyan;
    Color subColor = Theme::withAlpha(Theme::palette.son_bone, 200);
    char buf[128];

    std::snprintf(buf, sizeof(buf), "%s (X)", player1.getName().c_str());
    Fonts::draw(Fonts::bold, buf, hudX, hudY, 16, p1Color);
    std::snprintf(buf, sizeof(buf), "Wins: %d  Moves: %d",
                  player1.getWins(), player1.getMovesMade());
    Fonts::draw(Fonts::body, buf, hudX, hudY + 20, 13, subColor);

    DrawLineEx({static_cast<float>(hudX), static_cast<float>(hudY + 40)},
               {static_cast<float>(hudX + PANEL_W - 2 * PANEL_PAD), static_cast<float>(hudY + 40)},
               1.0f, Theme::withAlpha(Theme::palette.gold_foil, 80));

    std::snprintf(buf, sizeof(buf), "%s (O)", player2.getName().c_str());
    Fonts::draw(Fonts::bold, buf, hudX, hudY + 48, 16, p2Color);
    std::snprintf(buf, sizeof(buf), "Wins: %d  Moves: %d",
                  player2.getWins(), player2.getMovesMade());
    Fonts::draw(Fonts::body, buf, hudX, hudY + 68, 13, subColor);

    DrawLineEx({static_cast<float>(hudX), static_cast<float>(hudY + 88)},
               {static_cast<float>(hudX + PANEL_W - 2 * PANEL_PAD), static_cast<float>(hudY + 88)},
               1.0f, Theme::withAlpha(Theme::palette.gold_foil, 80));

    const char* turnText = isPlayer1Turn ? "Turn: X" : "Turn: O";
    Fonts::draw(Fonts::title, turnText, hudX, hudY + 96, 26, Theme::palette.gold_foil);

    std::snprintf(buf, sizeof(buf), "Move %d", totalMoves);
    Fonts::draw(Fonts::body, buf, hudX, hudY + 126, 13, subColor);
}

void GameScreen::drawMessage(const char* message) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    const float fontSize  = 36.0f;
    const int   textWidth = Fonts::measure(Fonts::title, message, fontSize);
    const int   padX      = 30;
    const int   bandH     = 60;
    const int   bandW     = textWidth + 2 * padX;
    const int   bandX     = (screenW - bandW) / 2;
    const int   bandY     = screenH - 80;

    // Ink-wash band across the bottom — gives the gold pixel text a backing
    // so it doesn't float over busy scenery.
    Rectangle band = {
        static_cast<float>(bandX), static_cast<float>(bandY),
        static_cast<float>(bandW), static_cast<float>(bandH)
    };
    DrawRectangleRec(band, Theme::withAlpha(Theme::palette.ink_sumi, 200));
    DrawRectangle(bandX, bandY,             bandW, 1, Theme::palette.gold_foil);
    DrawRectangle(bandX, bandY + bandH - 1, bandW, 1, Theme::palette.gold_foil);

    int textX = bandX + padX;
    int textY = bandY + (bandH - static_cast<int>(fontSize)) / 2;
    Fonts::draw(Fonts::title, message, textX, textY, fontSize, Theme::palette.gold_foil);
}

void GameScreen::drawPrompt(const char* /*prompt*/, char* /*buffer*/, int /*maxLen*/) {
    // TODO: implement text input prompt for save/load filenames (task 100)
}
