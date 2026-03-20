#include "Game.h"
#include <ctime>
#include <cstring>

static const int SCREEN_WIDTH = 1000;
static const int SCREEN_HEIGHT = 700;

Game::Game()
    : state(GameState::Menu),
      player1(nullptr), player2(nullptr), currentPlayer(nullptr),
      cursorRow(Board::SIZE / 2), cursorCol(Board::SIZE / 2),
      vsAI(true), aiDepth(4), playTime(0.0f),
      toastMessage{}, toastTimer(0.0f),
      aiThinking(false), aiResult{-1, -1} {}

Game::~Game() {
    if (aiThread.joinable()) aiThread.join();
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
            case GameState::Menu:       updateMenu();           break;
            case GameState::Settings:   updateSettings();       break;
            case GameState::Playing:    updatePlaying();        break;
            case GameState::GameOver:   updateGameOver();       break;
            case GameState::SaveScreen: // fallthrough
            case GameState::LoadScreen: updateSaveLoadScreen(); break;
        }

        // Draw
        BeginDrawing();
        ClearBackground({30, 30, 30, 255});

        switch (state) {
            case GameState::Menu:       drawMenu();           break;
            case GameState::Settings:   drawSettings();       break;
            case GameState::Playing:    drawPlaying();        break;
            case GameState::GameOver:   drawGameOver();       break;
            case GameState::SaveScreen: // fallthrough
            case GameState::LoadScreen: drawSaveLoadScreen(); break;
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
            saveLoadScreen.open(SlotScreenMode::Load);
            state = GameState::LoadScreen;
            menuScreen.reset();
            break;
        case MenuChoice::Settings:
            settingsScreen.setSettings({vsAI, aiDepth});
            settingsScreen.reset();
            state = GameState::Settings;
            menuScreen.reset();
            break;
        case MenuChoice::Exit:
            CloseWindow();
            break;
        default:
            break;
    }
}

void Game::updateSettings() {
    settingsScreen.update();
    if (settingsScreen.isDone()) {
        GameSettings s = settingsScreen.getSettings();
        vsAI = s.vsAI;
        aiDepth = s.aiDepth;
        state = GameState::Menu;
    }
}

void Game::updatePlaying() {
    playTime += GetFrameTime();
    if (toastTimer > 0.0f) toastTimer -= GetFrameTime();

    renderer.updateCamera();

    // Check if AI finished thinking
    if (aiThinking.load() == false && aiResult.row >= 0) {
        applyMove(aiResult);
        aiResult = {-1, -1};
        return;
    }

    // If AI is thinking, skip input but keep rendering
    if (aiThinking.load()) return;

    handleInput();

    // If current player is AI, spawn thread
    if (dynamic_cast<AIPlayer*>(currentPlayer) != nullptr) {
        aiThinking.store(true);
        if (aiThread.joinable()) aiThread.join();

        // Capture by value what the thread needs
        Board boardCopy = board;
        Player* ai = currentPlayer;
        aiThread = std::thread([this, boardCopy, ai]() {
            Board b = boardCopy;
            Move move = ai->getMove(b);
            aiResult = move;
            aiThinking.store(false);
        });
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

void Game::drawSettings() {
    settingsScreen.draw();
}

void Game::drawPlaying() {
    CellState turnMark = currentPlayer->getMark();
    renderer.drawBoard(board, cursorRow, cursorCol, turnMark);

    if (!winLine.empty()) {
        renderer.drawWinLine(winLine);
    }

    bool isP1Turn = (currentPlayer == player1);
    gameScreen.drawHUD(*player1, *player2, isP1Turn, board.getMoveCount());

    // Camera control buttons (2D overlay)
    renderer.drawCameraControls();

    // AI thinking indicator
    if (aiThinking.load()) {
        float pulse = (sinf(static_cast<float>(GetTime()) * 4.0f) + 1.0f) * 0.5f;
        auto alpha = static_cast<unsigned char>(150 + pulse * 105);
        const char* text = "AI is thinking...";
        int w = MeasureText(text, 24);
        DrawText(text, (GetScreenWidth() - w) / 2, GetScreenHeight() / 2 - 50, 24, {255, 215, 0, alpha});
    }

    // Save/Load buttons
    if (renderer.drawSaveButton()) {
        saveLoadScreen.open(SlotScreenMode::Save);
        state = GameState::SaveScreen;
    }
    if (renderer.drawLoadButton()) {
        saveLoadScreen.open(SlotScreenMode::Load);
        state = GameState::LoadScreen;
    }

    // Toast notification
    drawToast();
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
    if (aiThread.joinable()) aiThread.join();
    aiThinking.store(false);
    aiResult = {-1, -1};

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
    playTime = 0.0f;
    renderer.resetAnimations();
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
        // Don't place pieces when clicking UI buttons
        if (renderer.isPointOnUI(mousePos)) return;
        int row, col;
        if (renderer.screenToBoard(mousePos, row, col)) {
            if (board.isEmpty(row, col) && dynamic_cast<AIPlayer*>(currentPlayer) == nullptr) {
                applyMove({row, col});
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
            applyMove({cursorRow, cursorCol});
        }
    }

    // Save/Load shortcuts (Ctrl+S / Ctrl+L)
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
        if (IsKeyPressed(KEY_S)) {
            saveLoadScreen.open(SlotScreenMode::Save);
            state = GameState::SaveScreen;
        }
        if (IsKeyPressed(KEY_L)) {
            saveLoadScreen.open(SlotScreenMode::Load);
            state = GameState::LoadScreen;
        }
    }
}

void Game::updateSaveLoadScreen() {
    saveLoadScreen.update();
    SlotScreenResult res = saveLoadScreen.getResult();

    if (res == SlotScreenResult::Selected) {
        int slot = saveLoadScreen.getSelectedSlot();
        if (saveLoadScreen.getMode() == SlotScreenMode::Save) {
            performSave(slot);
            state = GameState::Playing;
        } else {
            performLoad(slot);
        }
    } else if (res == SlotScreenResult::Cancelled) {
        // Return to previous state
        if (player1 != nullptr) {
            state = GameState::Playing;
        } else {
            state = GameState::Menu;
            menuScreen.reset();
        }
    }
}

void Game::drawSaveLoadScreen() {
    saveLoadScreen.draw();
}

void Game::drawToast() {
    if (toastTimer > 0.0f) {
        int screenW = GetScreenWidth();
        int textWidth = MeasureText(toastMessage, 18);
        int x = screenW - textWidth - 20;
        int y = 20;
        float alpha = (toastTimer < 0.5f) ? toastTimer * 2.0f : 1.0f;
        auto a = static_cast<unsigned char>(alpha * 200);
        DrawRectangle(x - 10, y - 5, textWidth + 20, 30, {30, 30, 30, a});
        DrawText(toastMessage, x, y, 18, {255, 215, 0, static_cast<unsigned char>(alpha * 255)});
    }
}

void Game::buildSaveData(SaveData& data) {
    data = {};
    data.header.timestamp = static_cast<int64_t>(std::time(nullptr));
    data.header.playTime = playTime;
    data.header.moveCount = board.getMoveCount();
    data.header.gameMode = vsAI ? 1 : 0;
    data.header.aiDepth = aiDepth;
    data.header.currentTurn = (currentPlayer == player1) ? 1 : 2;
    data.header.p1Wins = player1->getWins();
    data.header.p2Wins = player2->getWins();
    data.header.p1Moves = player1->getMovesMade();
    data.header.p2Moves = player2->getMovesMade();
    std::strncpy(data.header.p1Name, player1->getName().c_str(), 31);
    std::strncpy(data.header.p2Name, player2->getName().c_str(), 31);

    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            data.cells[r][c] = static_cast<int>(board.getCell(r, c));
        }
    }
    Move last = board.getLastMove();
    data.lastMoveRow = last.row;
    data.lastMoveCol = last.col;
}

void Game::performSave(int slot) {
    SaveData data{};
    buildSaveData(data);
    if (FileManager::saveSlot(slot, data)) {
        std::snprintf(toastMessage, sizeof(toastMessage), "Saved to Slot %d", slot);
        toastTimer = 2.0f;
    } else {
        std::snprintf(toastMessage, sizeof(toastMessage), "Save failed!");
        toastTimer = 2.0f;
    }
}

void Game::performLoad(int slot) {
    SaveData data{};
    LoadResult res = FileManager::loadSlot(slot, data);
    if (res != LoadResult::OK) {
        std::snprintf(toastMessage, sizeof(toastMessage), "Load failed!");
        toastTimer = 2.0f;
        return;
    }

    // Restore settings
    vsAI = (data.header.gameMode == 1);
    aiDepth = data.header.aiDepth;
    playTime = data.header.playTime;

    // Recreate players
    delete player1;
    delete player2;
    player1 = new Player(data.header.p1Name, CellState::PlayerX);
    if (vsAI) {
        player2 = new AIPlayer(data.header.p2Name, CellState::PlayerO, aiDepth);
    } else {
        player2 = new Player(data.header.p2Name, CellState::PlayerO);
    }

    // Restore board
    board.reset();
    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            auto cell = static_cast<CellState>(data.cells[r][c]);
            if (cell != CellState::Empty) {
                board.placeMove(r, c, cell);
            }
        }
    }

    currentPlayer = (data.header.currentTurn == 1) ? player1 : player2;
    winLine.clear();
    state = GameState::Playing;

    const char* slotName = (slot == 0) ? "Autosave" : "Slot";
    std::snprintf(toastMessage, sizeof(toastMessage), "Loaded %s %d", slotName, slot);
    toastTimer = 2.0f;
}

void Game::applyMove(Move move) {
    if (board.placeMove(move.row, move.col, currentPlayer->getMark())) {
        currentPlayer->addMove();
        audioManager.playPlaceSound();
        autoSave();

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

void Game::autoSave() {
    if (player1 == nullptr) return;
    SaveData data{};
    buildSaveData(data);
    FileManager::saveSlot(0, data);
}
