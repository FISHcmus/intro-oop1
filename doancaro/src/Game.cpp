#include "Game.h"
#include "Fonts.h"
#include <climits>
#include <cstdlib>
#include <ctime>
#include <cstring>

static const int SCREEN_WIDTH = 1000;
static const int SCREEN_HEIGHT = 700;

Game::Game()
    : state(GameState::Menu), settingsReturnState(GameState::Menu),
      player1(nullptr), player2(nullptr), currentPlayer(nullptr),
      cursorRow(Board::SIZE / 2), cursorCol(Board::SIZE / 2),
      vsAI(true), aiDepth(3), playTime(0.0f),
      toastMessage{}, toastTimer(0.0f),
      showDebugPanel(false),
      aiThinking(false), aiResult{-1, -1} {
    loadSettings();
}

Game::~Game() {
    if (aiThread.joinable()) aiThread.join();
    delete player1;
    delete player2;
}

void Game::run() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Caro Game — OOP1 Project");
    SetTargetFPS(60);
    SetExitKey(0);  // Disable ESC auto-close; we handle ESC ourselves

    renderer.init(SCREEN_WIDTH, SCREEN_HEIGHT);
    Fonts::init();
    audioManager.init();
    MenuScreen::preload();

    while (!WindowShouldClose()) {
        // Settings/Save/Load fall through on purpose: they keep whatever's
        // already playing so a quick visit doesn't cut the current track.
        switch (state) {
            case GameState::Menu:
                audioManager.switchToMenuMusic();
                break;
            case GameState::Playing:
            case GameState::GameOver:
                audioManager.switchToGameMusic();
                break;
            default:
                break;
        }
        audioManager.updateMusic();

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
        ClearBackground({35, 28, 22, 255});  // warm dark wood tone

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

    // Stop AI engine before shutdown to prevent orphaned processes
    if (aiThread.joinable()) aiThread.join();
    auto* ai = dynamic_cast<AIPlayer*>(player2);
    delete player1; player1 = nullptr;
    delete player2; player2 = nullptr;

    audioManager.shutdown();
    Fonts::cleanup();
    renderer.shutdown();
    CloseWindow();
}

void Game::updateMenu() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        CloseWindow();
        return;
    }
    menuScreen.update(audioManager);
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
            settingsReturnState = GameState::Menu;
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
    settingsScreen.update(audioManager);
    if (settingsScreen.isDone()) {
        GameSettings s = settingsScreen.getSettings();
        vsAI = s.vsAI;
        aiDepth = s.aiDepth;
        saveSettings();

        // If returning to a game in progress, update existing AI player's depth
        if (settingsReturnState == GameState::Playing && player2 != nullptr) {
            auto* ai = dynamic_cast<AIPlayer*>(player2);
            if (ai != nullptr) ai->setSearchDepth(aiDepth);
        }

        state = settingsReturnState;
        settingsReturnState = GameState::Menu;
    }
}

void Game::updatePlaying() {
    playTime += GetFrameTime();
    if (toastTimer > 0.0f) toastTimer -= GetFrameTime();

    if (renderer.updateCamera()) {
        audioManager.playMenuClickSound();
    }
    renderer.uploadPendingTextures();
    renderer.updateParticles(GetFrameTime());

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
    renderer.updateParticles(GetFrameTime());

    if (IsKeyPressed(KEY_ENTER)) {
        audioManager.playMenuClickSound();
        startNewGame();
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
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
    renderer.drawVignette();

    if (!winLine.empty()) {
        renderer.drawWinLine(winLine);
    }

    bool isP1Turn = (currentPlayer == player1);
    gameScreen.drawHUD(*player1, *player2, isP1Turn, board.getMoveCount());

    // Camera control buttons (2D overlay)
    renderer.drawCameraControls();

    // AI thinking indicator — below HUD panel
    if (aiThinking.load()) {
        float pulse = (sinf(static_cast<float>(GetTime()) * 3.0f) + 1.0f) * 0.5f;
        auto alpha = static_cast<unsigned char>(140 + pulse * 115);

        int panelX = GetScreenWidth() - 210;
        int indicatorY = 248;

        // Animated dots: 1-3 dots cycling
        int dotCount = static_cast<int>(GetTime() * 2.0) % 3 + 1;
        char text[32];
        std::snprintf(text, sizeof(text), "AI thinking%.*s", dotCount, "...");

        Fonts::draw(Fonts::body, text, panelX + 12, indicatorY, 14, {255, 255, 255, alpha});
    }

    // Save/Load buttons
    if (renderer.drawSaveButton()) {
        audioManager.playMenuClickSound();
        saveLoadScreen.open(SlotScreenMode::Save);
        state = GameState::SaveScreen;
    }
    if (renderer.drawLoadButton()) {
        audioManager.playMenuClickSound();
        saveLoadScreen.open(SlotScreenMode::Load);
        state = GameState::LoadScreen;
    }

    // Menu/Settings buttons
    if (renderer.drawMenuButton()) {
        audioManager.playMenuClickSound();
        if (aiThread.joinable()) aiThread.join();
        aiThinking.store(false);
        aiResult = {-1, -1};
        menuScreen.reset();
        state = GameState::Menu;
    }
    if (renderer.drawSettingsButton()) {
        audioManager.playMenuClickSound();
        settingsScreen.setSettings({vsAI, aiDepth});
        settingsScreen.reset();
        settingsReturnState = GameState::Playing;
        state = GameState::Settings;
    }

    // Undo button
    if (renderer.drawUndoButton()) {
        audioManager.playMenuClickSound();
        undoLastMove();
    }

    // Restart button
    if (renderer.drawRestartButton()) {
        audioManager.playMenuClickSound();
        startNewGame();
    }

    // Debug panel
    if (showDebugPanel) drawDebugPanel();

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
    audioManager.stopGameOverSounds();

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
    moveHistory.clear();
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

    // ESC → return to menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        if (aiThread.joinable()) aiThread.join();
        aiThinking.store(false);
        aiResult = {-1, -1};
        menuScreen.reset();
        state = GameState::Menu;
        return;
    }

    // Debug panel toggle
    if (IsKeyPressed(KEY_F3)) {
        showDebugPanel = !showDebugPanel;
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
    saveLoadScreen.update(audioManager);
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
        int textWidth = Fonts::measure(Fonts::bold, toastMessage, 18);
        int x = screenW - textWidth - 20;
        int y = 20;
        float alpha = (toastTimer < 0.5f) ? toastTimer * 2.0f : 1.0f;
        auto a = static_cast<unsigned char>(alpha * 200);
        DrawRectangle(x - 10, y - 5, textWidth + 20, 30, {30, 30, 30, a});
        Fonts::draw(Fonts::bold, toastMessage, x, y, 18, {255, 215, 0, static_cast<unsigned char>(alpha * 255)});
    }
}

void Game::drawDebugPanel() {
    // Get debug info from AI player (if exists)
    auto* ai = dynamic_cast<AIPlayer*>(player2);
    if (!ai) ai = dynamic_cast<AIPlayer*>(player1);
    if (!ai) return;

    const auto& dbg = ai->getLastDebug();
    if (dbg.topMoves.empty() && dbg.reason.empty()) return;

    // Panel on the left side
    int px = 10;
    int py = 50;
    int pw = 280;
    int lineH = 18;
    int numLines = 8 + static_cast<int>(dbg.topMoves.size());
    int ph = 12 + numLines * lineH + 12;

    // Background
    DrawRectangleRounded({static_cast<float>(px), static_cast<float>(py),
                          static_cast<float>(pw), static_cast<float>(ph)},
                         0.08f, 4, {15, 15, 15, 210});
    DrawRectangleRoundedLinesEx({static_cast<float>(px), static_cast<float>(py),
                                  static_cast<float>(pw), static_cast<float>(ph)},
                                0.08f, 4, 1.0f, {100, 200, 255, 80});

    int tx = px + 10;
    int ty = py + 10;
    char buf[128];

    // Header
    Fonts::draw(Fonts::bold, "AI Debug [F3]", tx, ty, 15, {100, 200, 255, 255});
    ty += lineH;

    // Reason + depth + time
    std::snprintf(buf, sizeof(buf), "Mode: %s", dbg.reason.c_str());
    Fonts::draw(Fonts::body, buf, tx, ty, 13, {200, 200, 200, 255});
    ty += lineH;

    std::snprintf(buf, sizeof(buf), "Depth: %d  Candidates: %d  Time: %lldms",
                  dbg.depthCompleted, dbg.totalCandidates, dbg.searchTimeMs);
    Fonts::draw(Fonts::body, buf, tx, ty, 13, {200, 200, 200, 255});
    ty += lineH;

    // TT stats — hit rate = ttHits / ttProbes, cutoff rate = ttCutoffs / ttHits
    double hitPct = (dbg.ttProbes > 0)
                    ? (100.0 * static_cast<double>(dbg.ttHits) / static_cast<double>(dbg.ttProbes))
                    : 0.0;
    std::snprintf(buf, sizeof(buf), "Nodes: %lld  TT: %lld/%lld hits (%.1f%%)",
                  dbg.nodesSearched, dbg.ttHits, dbg.ttProbes, hitPct);
    Color ttColor = (dbg.ttHits > 0) ? Color{200, 200, 200, 255} : Color{255, 180, 120, 255};
    Fonts::draw(Fonts::body, buf, tx, ty, 13, ttColor);
    ty += lineH;

    std::snprintf(buf, sizeof(buf), "Cutoffs: %lld  Hoists: %lld  Stored: %d",
                  dbg.ttCutoffs, dbg.ttHoists, dbg.ttFinalSize);
    Fonts::draw(Fonts::body, buf, tx, ty, 13, ttColor);
    ty += lineH;

    // Chosen move
    std::snprintf(buf, sizeof(buf), "Chosen: (%d, %d)", dbg.chosenMove.row, dbg.chosenMove.col);
    Fonts::draw(Fonts::bold, buf, tx, ty, 13, {100, 255, 100, 255});
    ty += lineH;

    // Separator
    DrawLineEx({static_cast<float>(tx), static_cast<float>(ty + 2)},
               {static_cast<float>(tx + pw - 20), static_cast<float>(ty + 2)},
               1.0f, {255, 255, 255, 30});
    ty += lineH / 2;

    // Top moves header
    Fonts::draw(Fonts::bold, "Move       Pre    Search", tx, ty, 12, {180, 180, 180, 255});
    ty += lineH;

    // Top moves
    for (size_t i = 0; i < dbg.topMoves.size(); i++) {
        const auto& m = dbg.topMoves[i];
        bool isChosen = (m.move.row == dbg.chosenMove.row && m.move.col == dbg.chosenMove.col);
        Color rowColor = isChosen ? Color{100, 255, 100, 255} : Color{200, 200, 200, 220};

        if (m.searchScore == INT_MIN) {
            std::snprintf(buf, sizeof(buf), "(%2d,%2d)  %6d      --",
                          m.move.row, m.move.col, m.preScore);
        } else {
            std::snprintf(buf, sizeof(buf), "(%2d,%2d)  %6d  %6d",
                          m.move.row, m.move.col, m.preScore, m.searchScore);
        }
        Fonts::draw(Fonts::body, buf, tx, ty, 12, rowColor);
        ty += lineH;
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
    Move prevLast = board.getLastMove();
    if (board.placeMove(move.row, move.col, currentPlayer->getMark())) {
        moveHistory.push_back({move, currentPlayer->getMark(), prevLast});
        currentPlayer->addMove();
        audioManager.playPlaceSound();
        autoSave();

        winLine.clear();
        CellState winner = board.checkWinner(winLine);
        if (winner != CellState::Empty) {
            currentPlayer->addWin();
            state = GameState::GameOver;
            // PvAI: AI is player2. AI win ⇒ human lost ⇒ lose sound.
            if (vsAI && currentPlayer == player2) {
                audioManager.playLoseSound();
            } else {
                audioManager.playWinSound();
            }
        } else if (board.isFull()) {
            state = GameState::GameOver;
        } else {
            switchTurn();
        }
    }
}

void Game::undoLastMove() {
    if (moveHistory.empty()) return;
    if (aiThinking.load()) return;  // don't undo while AI is thinking

    audioManager.stopGameOverSounds();

    // In PvAI, undo two moves (AI + player) to get back to player's turn
    int undoCount = (vsAI && moveHistory.size() >= 2) ? 2 : 1;

    for (int i = 0; i < undoCount && !moveHistory.empty(); i++) {
        auto& rec = moveHistory.back();
        board.undoMove(rec.move.row, rec.move.col, rec.prevLastMove);

        // Decrement the move count for the player who made this move
        if (rec.mark == player1->getMark()) {
            player1->undoMove();
        } else {
            player2->undoMove();
        }

        moveHistory.pop_back();
        switchTurn();
    }

    // Clear win state if we were in game over
    winLine.clear();
    if (state == GameState::GameOver) {
        state = GameState::Playing;
    }

    // Reset renderer animation state for undone cells
    renderer.resetAnimations();
}

void Game::autoSave() {
    if (player1 == nullptr) return;
    SaveData data{};
    buildSaveData(data);
    FileManager::saveSlot(0, data);
}

void Game::saveSettings() const {
    FILE* f = fopen("settings.cfg", "w");
    if (!f) return;
    fprintf(f, "%d %d\n", vsAI ? 1 : 0, aiDepth);
    fclose(f);
}

void Game::loadSettings() {
    FILE* f = fopen("settings.cfg", "r");
    if (!f) return;
    char buf[64];
    if (fgets(buf, sizeof(buf), f)) {
        char* end = nullptr;
        long ai = strtol(buf, &end, 10);
        if (end && end != buf) {
            long depth = strtol(end, &end, 10);
            vsAI = (ai != 0);
            if (depth == 1 || depth == 2 || depth == 3) {
                aiDepth = static_cast<int>(depth);
            } else {
                aiDepth = 3;  // legacy 2/3/4/6/8 or any other value → Hard default
            }
        }
    }
    fclose(f);
}
