#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"
#include "AIPlayer.h"
#include "Renderer.h"
#include "MenuScreen.h"
#include "GameScreen.h"
#include "SettingsScreen.h"
#include "SaveLoadScreen.h"
#include "AudioManager.h"
#include "FileManager.h"
#include <thread>
#include <atomic>

enum class GameState { Menu, Settings, Playing, GameOver, SaveScreen, LoadScreen };

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    // State
    GameState state;
    GameState settingsReturnState;  // where to go after settings screen
    Board board;
    Player* player1;
    Player* player2;
    Player* currentPlayer;
    std::vector<Move> winLine;

    // Components
    Renderer renderer;
    MenuScreen menuScreen;
    SettingsScreen settingsScreen;
    SaveLoadScreen saveLoadScreen;
    GameScreen gameScreen;
    AudioManager audioManager;

    // Cursor (for keyboard navigation)
    int cursorRow;
    int cursorCol;

    // Settings
    bool vsAI;
    int aiDepth;

    // Play time tracking
    float playTime;

    // Toast notification
    char toastMessage[64];
    float toastTimer;

    // Debug panel
    bool showDebugPanel;

    // Move history for undo
    struct MoveRecord {
        Move move;
        CellState mark;
        Move prevLastMove;
    };
    std::vector<MoveRecord> moveHistory;

    // Async AI
    std::thread aiThread;
    std::atomic<bool> aiThinking;
    Move aiResult;

    // Game loop phases
    void updateMenu();
    void updateSettings();
    void updatePlaying();
    void updateGameOver();
    void updateSaveLoadScreen();

    void drawMenu();
    void drawSettings();
    void drawPlaying();
    void drawGameOver();
    void drawSaveLoadScreen();
    void drawToast();
    void drawDebugPanel();

    // Helpers
    void startNewGame();
    void switchTurn();
    void handleInput();
    void handleMouseInput();
    void handleKeyboardInput();
    void performSave(int slot);
    void performLoad(int slot);
    void autoSave();
    void buildSaveData(SaveData& data);
    void saveSettings() const;
    void loadSettings();
    void applyMove(Move move);
    void undoLastMove();
};

#endif
