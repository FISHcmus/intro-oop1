#include "GameScreen.h"
#include "raylib.h"
#include <cstdio>

GameScreen::GameScreen() : hudX(0), hudY(0) {}

void GameScreen::drawHUD(const Player& player1, const Player& player2,
                          bool isPlayer1Turn, int totalMoves) {
    int screenW = GetScreenWidth();
    hudX = screenW - 190;
    hudY = 60;

    DrawText("--- PLAYERS ---", hudX, hudY, 18, WHITE);

    // Player 1
    Color p1Color = isPlayer1Turn ? GOLD : LIGHTGRAY;
    char buf[128];

    std::snprintf(buf, sizeof(buf), "%s (X)", player1.getName().c_str());
    DrawText(buf, hudX, hudY + 30, 16, p1Color);
    std::snprintf(buf, sizeof(buf), "Wins: %d  Moves: %d",
                  player1.getWins(), player1.getMovesMade());
    DrawText(buf, hudX, hudY + 50, 14, p1Color);

    // Player 2
    Color p2Color = !isPlayer1Turn ? GOLD : LIGHTGRAY;
    std::snprintf(buf, sizeof(buf), "%s (O)", player2.getName().c_str());
    DrawText(buf, hudX, hudY + 80, 16, p2Color);
    std::snprintf(buf, sizeof(buf), "Wins: %d  Moves: %d",
                  player2.getWins(), player2.getMovesMade());
    DrawText(buf, hudX, hudY + 100, 14, p2Color);

    // Turn indicator
    const char* turnText = isPlayer1Turn ? "Turn: X" : "Turn: O";
    DrawText(turnText, hudX, hudY + 140, 20, GOLD);

    // Total moves
    std::snprintf(buf, sizeof(buf), "Total: %d", totalMoves);
    DrawText(buf, hudX, hudY + 165, 14, DARKGRAY);
}

void GameScreen::drawMessage(const char* message) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int textWidth = MeasureText(message, 30);
    DrawText(message, (screenW - textWidth) / 2, screenH - 60, 30, GOLD);
}

void GameScreen::drawPrompt(const char* /*prompt*/, char* /*buffer*/, int /*maxLen*/) {
    // TODO: implement text input prompt for save/load filenames (task 100)
}
