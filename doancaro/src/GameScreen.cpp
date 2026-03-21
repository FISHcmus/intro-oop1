#include "GameScreen.h"
#include "Fonts.h"
#include "raylib.h"
#include <cstdio>

GameScreen::GameScreen() : hudX(0), hudY(0) {}

void GameScreen::drawHUD(const Player& player1, const Player& player2,
                          bool isPlayer1Turn, int totalMoves) {
    int screenW = GetScreenWidth();

    // Panel dimensions
    static constexpr int PANEL_W = 200;
    static constexpr int PANEL_PAD = 12;
    static constexpr int PANEL_ROUND = 8;
    int panelX = screenW - PANEL_W - 10;
    int panelY = 50;
    int panelH = 190;

    hudX = panelX + PANEL_PAD;
    hudY = panelY + PANEL_PAD;

    // Dark semi-transparent panel background
    DrawRectangleRounded({static_cast<float>(panelX), static_cast<float>(panelY),
                          static_cast<float>(PANEL_W), static_cast<float>(panelH)},
                         0.1f, 4, {20, 20, 20, 200});
    // Subtle border
    DrawRectangleRoundedLinesEx({static_cast<float>(panelX), static_cast<float>(panelY),
                                 static_cast<float>(PANEL_W), static_cast<float>(panelH)},
                                0.1f, 4, 1.0f, {255, 255, 255, 40});

    // Player 1
    Color p1Color = isPlayer1Turn ? GOLD : LIGHTGRAY;
    char buf[128];

    std::snprintf(buf, sizeof(buf), "%s (X)", player1.getName().c_str());
    Fonts::draw(Fonts::bold, buf, hudX, hudY, 16, p1Color);
    std::snprintf(buf, sizeof(buf), "Wins: %d  Moves: %d",
                  player1.getWins(), player1.getMovesMade());
    Fonts::draw(Fonts::body, buf, hudX, hudY + 20, 13, p1Color);

    // Separator
    DrawLineEx({static_cast<float>(hudX), static_cast<float>(hudY + 40)},
               {static_cast<float>(hudX + PANEL_W - 2 * PANEL_PAD), static_cast<float>(hudY + 40)},
               1.0f, {255, 255, 255, 30});

    // Player 2
    Color p2Color = !isPlayer1Turn ? GOLD : LIGHTGRAY;
    std::snprintf(buf, sizeof(buf), "%s (O)", player2.getName().c_str());
    Fonts::draw(Fonts::bold, buf, hudX, hudY + 48, 16, p2Color);
    std::snprintf(buf, sizeof(buf), "Wins: %d  Moves: %d",
                  player2.getWins(), player2.getMovesMade());
    Fonts::draw(Fonts::body, buf, hudX, hudY + 68, 13, p2Color);

    // Separator
    DrawLineEx({static_cast<float>(hudX), static_cast<float>(hudY + 88)},
               {static_cast<float>(hudX + PANEL_W - 2 * PANEL_PAD), static_cast<float>(hudY + 88)},
               1.0f, {255, 255, 255, 30});

    // Turn indicator
    const char* turnText = isPlayer1Turn ? "Turn: X" : "Turn: O";
    Fonts::draw(Fonts::title, turnText, hudX, hudY + 96, 26, GOLD);

    // Total moves
    std::snprintf(buf, sizeof(buf), "Move %d", totalMoves);
    Fonts::draw(Fonts::body, buf, hudX, hudY + 126, 13, {180, 180, 180, 255});
}

void GameScreen::drawMessage(const char* message) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int textWidth = Fonts::measure(Fonts::title, message, 36);
    Fonts::draw(Fonts::title, message, (screenW - textWidth) / 2, screenH - 60, 36, GOLD);
}

void GameScreen::drawPrompt(const char* /*prompt*/, char* /*buffer*/, int /*maxLen*/) {
    // TODO: implement text input prompt for save/load filenames (task 100)
}
