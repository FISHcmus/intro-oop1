#include "Game.h"
#include "Fonts.h"
#include "Theme.h"
#include "UIComponents.h"
#include "StoryContent.h"
#include "StorySigil.h"
#include <climits>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>

static const int SCREEN_WIDTH = 1000;
static const int SCREEN_HEIGHT = 700;

namespace {

// Immediate-mode navigation button row. Draws up to 3 buttons centred at
// (screenW/2, yCenter) and returns the index of the one clicked this frame
// (-1 if none). Click only registers when `enabled` is true.
struct NavBtn {
    const char* label;
    bool        enabled;
};

int drawNavRow(int screenW, int yCenter, const NavBtn* btns, int count) {
    constexpr int kBtnW   = 220;
    constexpr int kBtnH   = 48;
    constexpr int kBtnGap = 18;

    int totalW = count * kBtnW + (count - 1) * kBtnGap;
    int x = (screenW - totalW) / 2;
    int y = yCenter - kBtnH / 2;

    int clicked = -1;
    Vector2 mp = GetMousePosition();
    for (int i = 0; i < count; ++i) {
        Rectangle r = { static_cast<float>(x), static_cast<float>(y),
                        static_cast<float>(kBtnW), static_cast<float>(kBtnH) };
        UIC::State s = UIC::State::Rest;
        if (!btns[i].enabled) {
            s = UIC::State::Disabled;
        } else if (CheckCollisionPointRec(mp, r)) {
            s = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? UIC::State::Pressed
                                                     : UIC::State::Focused;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) clicked = i;
        }
        UIC::drawPrimaryButton(r, btns[i].label, s);
        x += kBtnW + kBtnGap;
    }
    return clicked;
}

}  // namespace

Game::Game()
    : state(GameState::Menu), settingsReturnState(GameState::Menu),
      player1(nullptr), player2(nullptr), currentPlayer(nullptr),
      cursorRow(Board::SIZE / 2), cursorCol(Board::SIZE / 2),
      vsAI(true), aiDepth(3),
      inStoryMode(false),
      storySigilLastFillTime(-1.0f),
      playTime(0.0f),
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
        // Story narration screens use menu music — they're between matches.
        switch (state) {
            case GameState::Menu:
            case GameState::StoryIntro:
            case GameState::StoryBeat:
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
            case GameState::Menu:           updateMenu();           break;
            case GameState::Settings:       updateSettings();       break;
            case GameState::PickDifficulty: updateDifficulty();     break;
            case GameState::Playing:        updatePlaying();        break;
            case GameState::GameOver:       updateGameOver();       break;
            case GameState::SaveScreen: // fallthrough
            case GameState::LoadScreen:     updateSaveLoadScreen(); break;
            case GameState::StoryIntro:     updateStoryIntro();     break;
            case GameState::StoryBeat:      updateStoryBeat();      break;
        }

        // Draw
        BeginDrawing();
        ClearBackground(Theme::palette.sky_horizon);

        // 3-stop ink-wash sky behind the in-game 3D scene. Inline shader does
        // smoothstep-interpolation between sky_top / sky_mid / sky_horizon —
        // no Mach-band seam at the midline. Menu/Settings have their own
        // animated wuxia-storm BG, so only Playing/GameOver get the sky.
        if (state == GameState::Playing || state == GameState::GameOver) {
            renderer.drawSkyGradient();
        }

        switch (state) {
            case GameState::Menu:           drawMenu();           break;
            case GameState::Settings:       drawSettings();       break;
            case GameState::PickDifficulty: drawDifficulty();     break;
            case GameState::Playing:        drawPlaying();        break;
            case GameState::GameOver:       drawGameOver();       break;
            case GameState::SaveScreen: // fallthrough
            case GameState::LoadScreen:     drawSaveLoadScreen(); break;
            case GameState::StoryIntro:     drawStoryIntro();     break;
            case GameState::StoryBeat:      drawStoryBeat();      break;
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
    MenuScreen::shutdown();
    CloseWindow();
}

void Game::updateMenu() {
    if (toastTimer > 0.0f) toastTimer -= GetFrameTime();

    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        CloseWindow();
        return;
    }
    menuScreen.update(audioManager);
    MenuChoice choice = menuScreen.getChoice();

    switch (choice) {
        case MenuChoice::NewGame:
            if (vsAI) {
                difficultyScreen.reset();
                state = GameState::PickDifficulty;
                menuScreen.reset();
            } else {
                startNewGame();
            }
            break;
        case MenuChoice::StoryMode:
            inStoryMode = true;
            storyMode.reset();
            state = GameState::StoryIntro;
            menuScreen.reset();
            break;
        case MenuChoice::LoadGame:
            saveLoadScreen.open(SlotScreenMode::Load);
            state = GameState::LoadScreen;
            menuScreen.reset();
            break;
        case MenuChoice::Settings:
            settingsScreen.setSettings({vsAI});
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
        saveSettings();

        state = settingsReturnState;
        settingsReturnState = GameState::Menu;
    }
}

void Game::updateDifficulty() {
    difficultyScreen.update(audioManager);
    if (difficultyScreen.isDone()) {
        if (difficultyScreen.wasCancelled()) {
            state = GameState::Menu;
        } else {
            aiDepth = difficultyScreen.getChosenDepth();
            startNewGame();
        }
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
        if (inStoryMode && storyMode.consumeGaTurn()) {
            auto cands = board.getCandidateMoves();
            if (!cands.empty()) {
                aiResult = cands[std::rand() % cands.size()];
                return;  // applyMove picks it up next frame
            }
        }

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
        if (inStoryMode) {
            // Best-of-3: if the set is decided, route to the win/lose panel;
            // otherwise just start the next match in the set.
            if (storyMode.subBeat == StoryMode::SubBeat::SetWin ||
                storyMode.subBeat == StoryMode::SubBeat::SetLose) {
                state = GameState::StoryBeat;
            } else {
                startNewGame();
            }
        } else {
            startNewGame();
        }
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        if (inStoryMode) {
            inStoryMode = false;
            storyMode.reset();
        }
        state = GameState::Menu;
        menuScreen.reset();
    }
}

void Game::drawMenu() {
    menuScreen.draw();
    drawToast();
}

void Game::drawSettings() {
    settingsScreen.draw();
}

void Game::drawDifficulty() {
    difficultyScreen.draw();
}

void Game::drawPlaying() {
    CellState turnMark = currentPlayer->getMark();
    renderer.drawBoard(board, cursorRow, cursorCol, turnMark);
    renderer.drawMist();
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
        settingsScreen.setSettings({vsAI});
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

    // Story-mode overlays — linh vật charges + set score
    if (inStoryMode) drawStoryHUD();

    // Debug panel
    if (showDebugPanel) drawDebugPanel();

    // Toast notification
    drawToast();
}

void Game::drawGameOver() {
    // Draw the final board state with win line highlight. In Story Mode this
    // pass also runs drawStoryHUD → sigil + radial wash + caption, which IS
    // the announcement. The generic ink-band banner below would cover them.
    drawPlaying();

    if (inStoryMode) {
        // Story Mode: skip the generic banner — the sigil already announces
        // win/loss. Show two nav buttons above it (mid-set: "Trận sau",
        // set-decided: "Tiếp" routes to the SetWin/SetLose narrative panel).
        // Buttons sit at h-150 to clear the sigil apex (~h-81).
        bool setDecided = (storyMode.subBeat == StoryMode::SubBeat::SetWin ||
                           storyMode.subBeat == StoryMode::SubBeat::SetLose);
        NavBtn btns[2] = {
            { "Menu", true },
            { "Next", true },
        };
        int clicked = drawNavRow(GetScreenWidth(), GetScreenHeight() - 150,
                                 btns, 2);
        if (clicked == 0) {
            audioManager.playMenuClickSound();
            exitStoryToMenu();
            menuScreen.reset();
        } else if (clicked == 1) {
            audioManager.playMenuClickSound();
            if (setDecided) {
                state = GameState::StoryBeat;
            } else {
                startNewGame();
            }
        }
        return;
    }

    // Free-play / vs-AI / vs-human: keep the original ink-band banner.
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

    if (inStoryMode) {
        storyMode.onMatchStart();
        // New match starts fresh — drop the stale pulse/wash/caption from
        // the previous match-end. The sigil keeps its filled orbs visible
        // (matchOutcomes is set-scoped, only resets on SetIntro advance).
        storySigilLastFillTime = -1.0f;
    }

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

void Game::startStoryMatch() {
    vsAI = true;
    aiDepth = storyMode.getCurrentDifficulty();
    startNewGame();
}

void Game::exitStoryToMenu() {
    inStoryMode = false;
    storyMode.reset();
    state = GameState::Menu;
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

    // ---- Story Mode linh vật hotkeys ----
    if (inStoryMode && !aiThinking.load()) {
        // 1 = Voi 9 ngà — undo 5 player turns (10 board moves in PvAI).
        if (IsKeyPressed(KEY_ONE) && storyMode.useVoi()) {
            undoTurns(5);
            std::snprintf(toastMessage, sizeof(toastMessage),
                          "Voi 9 nga - hoan tac 5 nuoc co");
            toastTimer = 2.5f;
        }
        if (IsKeyPressed(KEY_TWO) && storyMode.useGa()) {
            std::snprintf(toastMessage, sizeof(toastMessage),
                          "Ga 9 cua gay! Doi thu loan tri 1 luot");
            toastTimer = 2.5f;
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

// ---- Story Mode screens ----

namespace {

// Map current set → its SetText block. Centralises the switch so update/draw
// don't both repeat it.
const StoryContent::SetText& setTextFor(StoryMode::SetId id) {
    switch (id) {
        case StoryMode::SetId::Set1:      return StoryContent::kSet1;
        case StoryMode::SetId::Set2:      return StoryContent::kSet2;
        case StoryMode::SetId::Set3:      return StoryContent::kSet3;
        case StoryMode::SetId::FinalBoss: return StoryContent::kFinalBoss;
    }
    return StoryContent::kSet1;
}

const char* linhVatUnlockLineFor(StoryMode::SetId id) {
    switch (id) {
        case StoryMode::SetId::Set1: return StoryContent::kVoiUnlockLine;
        case StoryMode::SetId::Set2: return StoryContent::kGaUnlockLine;
        case StoryMode::SetId::Set3: return StoryContent::kNguaUnlockLine;
        case StoryMode::SetId::FinalBoss: return "";
    }
    return "";
}

const char* linhVatNameFor(StoryMode::SetId id) {
    switch (id) {
        case StoryMode::SetId::Set1: return "VOI 9 NGÀ";
        case StoryMode::SetId::Set2: return "GÀ 9 CỰA";
        case StoryMode::SetId::Set3: return "NGỰA 9 HỒNG MAO";
        case StoryMode::SetId::FinalBoss: return "";
    }
    return "";
}

}  // namespace

void Game::updateStoryIntro() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        exitStoryToMenu();
        return;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        audioManager.playMenuClickSound();
        storyMode.advance();
        if (storyMode.subBeat == StoryMode::SubBeat::SetIntro) {
            state = GameState::StoryBeat;
        }
    }
}

void Game::drawStoryIntro() {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    DrawRectangleGradientV(0, 0, w, h,
                           Theme::palette.bg_top, Theme::palette.bg_bottom);

    UIC::drawTitle("CÔ SỬ TIÊN", w, h, 30);

    char tag[32];
    std::snprintf(tag, sizeof(tag), "TRANG %d/%d",
                  storyMode.introPageIdx + 1, StoryContent::kIntroPageCount);

    UIC::ComicPanel cp = {
        "MỞ ĐẦU",
        tag,
        nullptr,
        StoryContent::kIntroPages[storyMode.introPageIdx],
        62
    };
    UIC::drawComicPanel(cp, w / 2, 110);

    NavBtn btns[3] = {
        { "Prev", storyMode.introPageIdx > 0 },
        { "Skip", true },
        { "Next", true },
    };
    int clicked = drawNavRow(w, h - 110, btns, 3);
    if (clicked == 0) {
        audioManager.playMenuClickSound();
        if (storyMode.introPageIdx > 0) --storyMode.introPageIdx;
    } else if (clicked == 1) {
        // Skip: jump straight into Set1 match. Chain advance() twice —
        // IntroMonologue→SetIntro then SetIntro→MatchPlaying — and start
        // the match immediately so the player skips ALL narrative panels.
        audioManager.playMenuClickSound();
        storyMode.introPageIdx = StoryContent::kIntroPageCount - 1;
        storyMode.advance();   // → SetIntro
        storyMode.advance();   // → MatchPlaying
        startStoryMatch();
    } else if (clicked == 2) {
        audioManager.playMenuClickSound();
        storyMode.advance();
        if (storyMode.subBeat == StoryMode::SubBeat::SetIntro) {
            state = GameState::StoryBeat;
        }
    }

    UIC::drawHintBar("ENTER tiếp · ESC về menu", w, h);
}

void Game::updateStoryBeat() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        exitStoryToMenu();
        return;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        audioManager.playMenuClickSound();

        // Epilogue is a leaf — Enter exits to Menu without advancing.
        if (storyMode.subBeat == StoryMode::SubBeat::Epilogue) {
            exitStoryToMenu();
            return;
        }

        storyMode.advance();

        // After advance: route GameState by the new subBeat.
        if (storyMode.subBeat == StoryMode::SubBeat::MatchPlaying) {
            startStoryMatch();
        }
        // Otherwise stay on StoryBeat — next render shows the new panel.
    }
}

void Game::drawStoryBeat() {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    DrawRectangleGradientV(0, 0, w, h,
                           Theme::palette.bg_top, Theme::palette.bg_bottom);

    const char* hint = "ENTER tiếp · ESC về menu";
    UIC::ComicPanel cp = { nullptr, nullptr, nullptr, "", 62 };

    switch (storyMode.subBeat) {
        case StoryMode::SubBeat::SetIntro: {
            const auto& st = setTextFor(storyMode.currentSet);
            UIC::drawTitle(st.title, w, h, 30);
            cp.title = "VÀO TRẬN";
            cp.tag   = st.tag;
            cp.body  = st.intro;
            // Final boss reveals the kraken art in the intro.
            if (storyMode.currentSet == StoryMode::SetId::FinalBoss) {
                cp.plot = StoryContent::kThuyTinhBossArt;
            }
            hint = "ENTER bắt đầu trận · ESC về menu";
            break;
        }
        case StoryMode::SubBeat::SetWin: {
            const auto& st = setTextFor(storyMode.currentSet);
            UIC::drawTitle(st.title, w, h, 30);
            cp.title = "THẮNG";
            cp.tag   = st.tag;
            cp.body  = st.win;
            hint = "ENTER tiếp";
            break;
        }
        case StoryMode::SubBeat::SetLose: {
            const auto& st = setTextFor(storyMode.currentSet);
            UIC::drawTitle(st.title, w, h, 30);
            cp.title = "BẠI";
            cp.tag   = st.tag;
            cp.body  = st.lose;
            hint = "ENTER chơi lại set";
            break;
        }
        case StoryMode::SubBeat::LinhVatUnlock: {
            UIC::drawTitle("LINH VẬT", w, h, 30);
            cp.title = "BAN THƯỞNG";
            cp.tag   = linhVatNameFor(storyMode.currentSet);
            cp.body  = linhVatUnlockLineFor(storyMode.currentSet);
            hint = "ENTER tiếp · ESC về menu";
            break;
        }
        case StoryMode::SubBeat::Epilogue: {
            UIC::drawTitle("HỒI KẾT", w, h, 30);
            cp.title = "CÔ SỬ TIÊN";
            cp.tag   = "VĨ THANH";
            cp.body  = StoryContent::kEpilogueLine;
            hint = "ENTER về menu";
            break;
        }
        default:
            // MatchPlaying / IntroMonologue should not reach drawStoryBeat.
            break;
    }

    UIC::drawComicPanel(cp, w / 2, 110);

    // Epilogue is a leaf — only Menu is shown; all other beats add Next.
    int btnCount = (storyMode.subBeat == StoryMode::SubBeat::Epilogue) ? 1 : 2;
    NavBtn btns[2] = { { "Menu", true }, { "Next", true } };
    int clicked = drawNavRow(w, h - 110, btns, btnCount);
    if (clicked == 0) {
        audioManager.playMenuClickSound();
        exitStoryToMenu();
    } else if (clicked == 1) {
        audioManager.playMenuClickSound();
        storyMode.advance();
        if (storyMode.subBeat == StoryMode::SubBeat::MatchPlaying) {
            startStoryMatch();
        }
    }

    UIC::drawHintBar(hint, w, h);
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

void Game::drawStoryHUD() {
    const auto& st = setTextFor(storyMode.currentSet);

    // Top-left: set badge with best-of-3 score (boss is best-of-1).
    char badge[96];
    if (storyMode.currentSet == StoryMode::SetId::FinalBoss) {
        std::snprintf(badge, sizeof(badge), "%s · %s",
                      st.tag, st.title);
    } else {
        std::snprintf(badge, sizeof(badge), "%s · %s · TRAN %d-%d",
                      st.tag, st.title,
                      storyMode.matchWinsInSet, storyMode.matchLossesInSet);
    }
    DrawRectangle(12, 12, Fonts::measure(Fonts::bold, badge, 16) + 24, 28,
                  Theme::withAlpha(Theme::palette.ink_sumi, 180));
    Fonts::draw(Fonts::bold, badge, 24, 18, 16,
                Theme::withAlpha(Theme::palette.gold_foil, 240));

    // Bottom-left: linh vật strip with charges + hotkeys.
    auto label = [&](StoryMode::LinhVat lv, char* out, size_t n) -> const char* {
        if (!storyMode.isUnlocked(lv))            return "khoa";
        int c = storyMode.chargesLeft(lv);
        if (c <= 0)                               return "het";
        std::snprintf(out, n, "x%d", c);
        return out;
    };
    char voiBuf[8], gaBuf[8], nguaBuf[8];
    char strip[160];
    std::snprintf(strip, sizeof(strip),
                  "[1] VOI %s  [2] GA %s  [3] NGUA %s (tu dong)",
                  label(StoryMode::LinhVat::Voi,  voiBuf,  sizeof(voiBuf)),
                  label(StoryMode::LinhVat::Ga,   gaBuf,   sizeof(gaBuf)),
                  label(StoryMode::LinhVat::Ngua, nguaBuf, sizeof(nguaBuf)));

    // Bottom-RIGHT — bottom-left is reserved for camera buttons (Renderer).
    int sw = Fonts::measure(Fonts::body, strip, 14);
    int sx = GetScreenWidth() - sw - 24;
    int sy = GetScreenHeight() - 30;  // align with camera-button row baseline
    DrawRectangle(sx - 8, sy - 4, sw + 16, 22,
                  Theme::withAlpha(Theme::palette.ink_sumi, 170));
    Fonts::draw(Fonts::body, strip, sx, sy, 14,
                Theme::withAlpha(Theme::palette.son_bone, 230));

    // Middle-bottom: tam-thai sigil. Three orbs track best-of-3 outcomes;
    // pulses + radial screen-wash + caption all key off storySigilLastFillTime
    // captured at onMatchEnd.
    StorySigil::Layout sigil{};
    sigil.centerX = GetScreenWidth() / 2;
    sigil.bottomY = GetScreenHeight() - 12;
    auto now = static_cast<float>(GetTime());

    // Wash and caption durations from StorySigil.cpp's anon namespace —
    // duplicated here to skip work after they expire (otherwise snprintf
    // formats every frame even when drawCaption returns early).
    constexpr float kWashDur    = 0.7f;
    constexpr float kCaptionDur = 1.5f;

    bool sigilActive = (storySigilLastFillTime > 0.0f &&
                        storyMode.matchesPlayedInSet > 0);
    float sigilT = sigilActive ? (now - storySigilLastFillTime) : 0.0f;
    StoryMode::OrbState last = sigilActive
        ? storyMode.matchOutcomes[storyMode.matchesPlayedInSet - 1]
        : StoryMode::OrbState::Pending;

    if (sigilActive && sigilT <= kWashDur) {
        Color washColor = (last == StoryMode::OrbState::Won)
                              ? Color{110, 220, 130, 255}
                              : Color{220,  80,  80, 255};
        // Wash centered on the triangle centroid (1/3 above bottomY).
        int washY = sigil.bottomY -
                    static_cast<int>(static_cast<float>(sigil.sideLen) * 0.866f / 3.0f);
        StorySigil::drawScreenWash(sigil.centerX, washY,
                                   GetScreenWidth(), GetScreenHeight(),
                                   washColor, storySigilLastFillTime, now);
    }

    StorySigil::draw(sigil, storyMode.matchOutcomes, now,
                     storySigilLastFillTime);

    if (sigilActive && sigilT <= kCaptionDur) {
        char cap[40];
        std::snprintf(cap, sizeof(cap), "%s TRẬN %d",
                      last == StoryMode::OrbState::Won ? "THẮNG" : "BẠI",
                      storyMode.matchesPlayedInSet);
        StorySigil::drawCaption(sigil, cap, storySigilLastFillTime, now);
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
        const bool hasWinner = (winner != CellState::Empty);
        const bool isDraw    = !hasWinner && board.isFull();
        if (hasWinner || isDraw) {
            // PvAI: AI is player2. The mover's identity decides side.
            const bool aiWon      = hasWinner && vsAI && currentPlayer == player2;
            const bool playerWon  = hasWinner && !aiWon;
            const bool playerLost = aiWon || isDraw;

            // Story mode: Ngựa 9 hồng mao revives the player once per run.
            if (inStoryMode && playerLost && storyMode.tryUseNguaOnLoss()) {
                undoTurns(5);
                winLine.clear();
                std::snprintf(toastMessage, sizeof(toastMessage),
                              "Ngua 9 hong mao dua nguoi ve 5 luot truoc");
                toastTimer = 3.0f;
                return;
            }

            if (hasWinner) currentPlayer->addWin();
            state = GameState::GameOver;
            if (aiWon)            audioManager.playLoseSound();
            else if (playerWon)   audioManager.playWinSound();
            if (inStoryMode) {
                storyMode.onMatchEnd(playerWon);
                // Capture fill timestamp NOW so the sigil pulse, radial
                // screen-wash, and caption all share one t0.
                storySigilLastFillTime = static_cast<float>(GetTime());
            }
        } else {
            switchTurn();

            // FinalBoss cheating power: every 4 player moves, Thủy Tinh tears
            // 4 of the player's stones off the board.
            if (inStoryMode &&
                storyMode.currentSet == StoryMode::SetId::FinalBoss &&
                currentPlayer == player2 &&
                storyMode.tickBossCheat()) {
                int removed = 0;
                for (int i = static_cast<int>(moveHistory.size()) - 1;
                     i >= 0 && removed < 4; --i) {
                    if (moveHistory[i].mark == player1->getMark()) {
                        board.removeMove(moveHistory[i].move.row,
                                         moveHistory[i].move.col);
                        moveHistory.erase(moveHistory.begin() + i);
                        ++removed;
                    }
                }
                if (removed > 0) {
                    std::snprintf(toastMessage, sizeof(toastMessage),
                                  "Thuy Tinh xe ban co - %d quan bay mat!",
                                  removed);
                    toastTimer = 3.0f;
                }
            }
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

void Game::undoTurns(int n) {
    for (int i = 0; i < n && !moveHistory.empty(); ++i) {
        undoLastMove();
    }
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
    fprintf(f, "%d\n", vsAI ? 1 : 0);
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
            vsAI = (ai != 0);
        }
    }
    fclose(f);
}
