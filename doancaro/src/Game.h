#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"
#include "AIPlayer.h"
#include "Renderer.h"
#include "MenuScreen.h"
#include "GameScreen.h"
#include "AudioManager.h"
#include "FileManager.h"

enum class GameState { Menu, Playing, GameOver };

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    // State
    GameState state;
    Board board;
    Player* player1;
    Player* player2;
    Player* currentPlayer;
    std::vector<Move> winLine;

    // Components
    Renderer renderer;
    MenuScreen menuScreen;
    GameScreen gameScreen;
    AudioManager audioManager;

    // Cursor (for keyboard navigation)
    int cursorRow;
    int cursorCol;

    // Settings
    bool vsAI;
    int aiDepth;

    // Game loop phases
    void updateMenu();
    void updatePlaying();
    void updateGameOver();

    void drawMenu();
    void drawPlaying();
    void drawGameOver();

    // Helpers
    void startNewGame();
    void switchTurn();
    void handleInput();
    void handleMouseInput();
    void handleKeyboardInput();
    void saveGame();
    void loadGame();
};

#endif
