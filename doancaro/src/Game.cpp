#include "Game.h"

static const int SCREEN_WIDTH = 1000;
static const int SCREEN_HEIGHT = 700;

Game::Game()
    : state(GameState::Menu),
      player1(nullptr), player2(nullptr), currentPlayer(nullptr),
      cursorRow(Board::SIZE / 2), cursorCol(Board::SIZE / 2),
      vsAI(true), aiDepth(4) {}

Game::~Game() {
    delete player1;
    delete player2;
}

void Game::run() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Caro Game — OOP1 Project");
    SetTargetFPS(60);

    renderer.init(SCREEN_WIDTH, SCREEN_HEIGHT);
    audioManager.init();

    while (!WindowShouldClose()) {
        // Update
        switch (state) {
            case GameState::Menu:     updateMenu();     break;
            case GameState::Playing:  updatePlaying();  break;
            case GameState::GameOver: updateGameOver(); break;
        }

        // Draw
        BeginDrawing();
        ClearBackground({30, 30, 30, 255});

        switch (state) {
            case GameState::Menu:     drawMenu();     break;
            case GameState::Playing:  drawPlaying();  break;
            case GameState::GameOver: drawGameOver(); break;
        }

        EndDrawing();
    }

    audioManager.shutdown();
    renderer.shutdown();
    CloseWindow();
}

void Game::updateMenu() {
    menuScreen.update();
    MenuChoice choice = menuScreen.getChoice();

    switch (choice) {
        case MenuChoice::NewGame:
            startNewGame();
            break;
        case MenuChoice::LoadGame:
            loadGame();
            break;
        case MenuChoice::Exit:
            CloseWindow();
            break;
        default:
            break;
    }
}

void Game::updatePlaying() {
    renderer.updateCamera();
    handleInput();

    // If current player is AI, get AI move
    if (dynamic_cast<AIPlayer*>(currentPlayer) != nullptr) {
        Move aiMove = currentPlayer->getMove(board);
        if (board.placeMove(aiMove.row, aiMove.col, currentPlayer->getMark())) {
            currentPlayer->addMove();
            audioManager.playPlaceSound();

            // Check win
            winLine.clear();
            CellState winner = board.checkWinner(winLine);
            if (winner != CellState::Empty) {
                currentPlayer->addWin();
                state = GameState::GameOver;
                audioManager.playWinSound();
            } else if (board.isFull()) {
                state = GameState::GameOver;
            } else {
                switchTurn();
            }
        }
    }
}

void Game::updateGameOver() {
    if (IsKeyPressed(KEY_ENTER)) {
        startNewGame();
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::Menu;
        menuScreen.reset();
    }
}

void Game::drawMenu() {
    menuScreen.draw();
}

void Game::drawPlaying() {
    CellState turnMark = currentPlayer->getMark();
    renderer.drawBoard(board, cursorRow, cursorCol, turnMark);

    if (!winLine.empty()) {
        renderer.drawWinLine(winLine);
    }

    bool isP1Turn = (currentPlayer == player1);
    gameScreen.drawHUD(*player1, *player2, isP1Turn, board.getMoveCount());
}

void Game::drawGameOver() {
    // Draw the final board state with win line highlight
    drawPlaying();

    // Overlay message — use cached winLine from when game ended
    if (!winLine.empty()) {
        CellState winner = board.getCell(winLine[0].row, winLine[0].col);
        const char* winnerName = (winner == player1->getMark())
                                     ? player1->getName().c_str()
                                     : player2->getName().c_str();
        char msg[128];
        std::snprintf(msg, sizeof(msg), "%s wins! Enter=New Game, ESC=Menu", winnerName);
        gameScreen.drawMessage(msg);
    } else {
        gameScreen.drawMessage("Draw! Enter=New Game, ESC=Menu");
    }
}

void Game::startNewGame() {
    delete player1;
    delete player2;

    player1 = new Player("Player 1", CellState::PlayerX);
    if (vsAI) {
        player2 = new AIPlayer("AI", CellState::PlayerO, aiDepth);
    } else {
        player2 = new Player("Player 2", CellState::PlayerO);
    }

    board.reset();
    winLine.clear();
    currentPlayer = player1;
    cursorRow = Board::SIZE / 2;
    cursorCol = Board::SIZE / 2;
    state = GameState::Playing;
}

void Game::switchTurn() {
    currentPlayer = (currentPlayer == player1) ? player2 : player1;
}

void Game::handleInput() {
    handleKeyboardInput();
    handleMouseInput();
}

void Game::handleMouseInput() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        int row, col;
        if (renderer.screenToBoard(mousePos, row, col)) {
            if (board.isEmpty(row, col) && dynamic_cast<AIPlayer*>(currentPlayer) == nullptr) {
                if (board.placeMove(row, col, currentPlayer->getMark())) {
                    currentPlayer->addMove();
                    audioManager.playPlaceSound();

                    winLine.clear();
                    CellState winner = board.checkWinner(winLine);
                    if (winner != CellState::Empty) {
                        currentPlayer->addWin();
                        state = GameState::GameOver;
                        audioManager.playWinSound();
                    } else if (board.isFull()) {
                        state = GameState::GameOver;
                    } else {
                        switchTurn();
                    }
                }
            }
        }
    }
}

void Game::handleKeyboardInput() {
    // Cursor movement
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        if (cursorRow > 0) cursorRow--;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        if (cursorRow < Board::SIZE - 1) cursorRow++;
    }
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        if (cursorCol > 0) cursorCol--;
    }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        if (cursorCol < Board::SIZE - 1) cursorCol++;
    }

    // Place piece with Enter
    if (IsKeyPressed(KEY_ENTER)) {
        if (dynamic_cast<AIPlayer*>(currentPlayer) == nullptr
            && board.isEmpty(cursorRow, cursorCol)) {
            if (board.placeMove(cursorRow, cursorCol, currentPlayer->getMark())) {
                currentPlayer->addMove();
                audioManager.playPlaceSound();

                winLine.clear();
                CellState winner = board.checkWinner(winLine);
                if (winner != CellState::Empty) {
                    currentPlayer->addWin();
                    state = GameState::GameOver;
                    audioManager.playWinSound();
                } else if (board.isFull()) {
                    state = GameState::GameOver;
                } else {
                    switchTurn();
                }
            }
        }
    }

    // Save/Load shortcuts
    if (IsKeyPressed(KEY_L)) saveGame();
    if (IsKeyPressed(KEY_T)) loadGame();
}

void Game::saveGame() {
    // TODO: prompt for filename (task 100)
    GameSaveData data;
    data.currentTurnIsPlayer1 = (currentPlayer == player1);
    data.player1Wins = player1->getWins();
    data.player2Wins = player2->getWins();
    data.player1Moves = player1->getMovesMade();
    data.player2Moves = player2->getMovesMade();
    data.player1Name = player1->getName();
    data.player2Name = player2->getName();
    FileManager::saveGame("savegame.dat", board, data);
}

void Game::loadGame() {
    // TODO: prompt for filename (task 100)
    if (!FileManager::fileExists("savegame.dat")) return;

    GameSaveData data;
    if (FileManager::loadGame("savegame.dat", board, data)) {
        delete player1;
        delete player2;

        player1 = new Player(data.player1Name, CellState::PlayerX);
        if (vsAI) {
            player2 = new AIPlayer(data.player2Name, CellState::PlayerO, aiDepth);
        } else {
            player2 = new Player(data.player2Name, CellState::PlayerO);
        }

        currentPlayer = data.currentTurnIsPlayer1 ? player1 : player2;
        winLine.clear();
        state = GameState::Playing;
    }
}
