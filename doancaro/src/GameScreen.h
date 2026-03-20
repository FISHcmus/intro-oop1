#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include "Player.h"

class GameScreen {
public:
    GameScreen();

    void drawHUD(const Player& player1, const Player& player2,
                 bool isPlayer1Turn, int totalMoves);
    void drawMessage(const char* message);
    void drawPrompt(const char* prompt, char* buffer, int maxLen);

private:
    int hudX;
    int hudY;
};

#endif
