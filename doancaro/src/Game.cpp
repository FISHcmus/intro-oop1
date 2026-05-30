#include "Game.h"
#include "Fonts.h"
#include "Theme.h"
#include "UIComponents.h"
#include "StoryContent.h"
#include "StorySigil.h"
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>

static const int SCREEN_WIDTH = 1000;
static const int SCREEN_HEIGHT = 700;

namespace {

constexpr float kStoryPanelLineStep = 22.0f;
constexpr float kStoryPanelScrollStep = kStoryPanelLineStep * 2.0f;
constexpr float kStoryNavCenterFromBottom = 86.0f;
constexpr float kStoryNavButtonHeight = 48.0f;
constexpr float kStoryPanelGapToNav = 8.0f;
constexpr int kStoryPanelWidthCh = 70;

float storyNavRowCenterY(int screenH) {
    return static_cast<float>(screenH) - kStoryNavCenterFromBottom;
}

float storyPanelMaxHeightFor(int panelY, int screenH) {
    float navTop = storyNavRowCenterY(screenH) - kStoryNavButtonHeight * 0.5f;
    return std::max(kStoryPanelLineStep, navTop - static_cast<float>(panelY)
                                         - kStoryPanelGapToNav);
}

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

// Linh-vật trigger card. `vivid` lifts the card out of the dimmed look
// reserved for locked / spent beasts; `clickable` gates click registration
// on top of `interactive` (set by caller for the current GameState).
// Ngựa is always !clickable - it auto-fires on match loss.
struct BeastCard {
    const char* hotkey;
    const char* name;
    const char* hint;
    const char* chargeText;
    const Texture2D* icon;
    bool        clickable;
    bool        vivid;
};

int drawBeastCardRow(int rightX, int bottomY,
                     const BeastCard cards[3], bool interactive) {
    constexpr int kCardW = 126;
    constexpr int kCardH = 98;
    constexpr int kGap   = 8;
    int totalW = 3 * kCardW + 2 * kGap;
    int leftX  = rightX - totalW;
    int topY   = bottomY - kCardH;

    int clicked = -1;
    Vector2 mp = GetMousePosition();
    for (int i = 0; i < 3; ++i) {
        int x0 = leftX + i * (kCardW + kGap);
        int y0 = topY;
        int cx = x0 + kCardW / 2;
        Rectangle r = { static_cast<float>(x0), static_cast<float>(y0),
                        static_cast<float>(kCardW), static_cast<float>(kCardH) };

        bool hot = interactive && cards[i].clickable
                && CheckCollisionPointRec(mp, r);
        if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) clicked = i;

        unsigned char bgA      = cards[i].vivid ? 210 : 140;
        unsigned char borderA  = cards[i].vivid ? (hot ? 240 : 170) : 80;
        unsigned char nameA    = cards[i].vivid ? 240 : 130;
        unsigned char hintA    = cards[i].vivid ? 200 : 110;
        unsigned char chargeA  = cards[i].vivid ? 230 : 150;

        DrawRectangleRounded(r, 0.18f, 6,
                             Theme::withAlpha(Theme::palette.ink_sumi, bgA));
        DrawRectangleRoundedLinesEx(r, 0.18f, 6, 2.0f,
                                    Theme::withAlpha(Theme::palette.gold_foil, borderA));

        Rectangle badge = { static_cast<float>(x0 + 6), static_cast<float>(y0 + 6),
                            18.0f, 18.0f };
        DrawRectangleRounded(badge, 0.4f, 4,
                             Theme::withAlpha(Theme::palette.gold_foil,
                                              cards[i].vivid ? 255 : 150));
        Fonts::drawCentered(Fonts::bold, cards[i].hotkey,
                            x0 + 6 + 9, y0 + 8, 13.0f, Theme::palette.ink_sumi);

        int chW = Fonts::measure(Fonts::body, cards[i].chargeText, 12);
        Fonts::draw(Fonts::body, cards[i].chargeText,
                    x0 + kCardW - chW - 8, y0 + 8,
                    12.0f, Theme::withAlpha(Theme::palette.gold_foil, chargeA));

        if (cards[i].icon && cards[i].icon->id != 0) {
            constexpr float kIconSize = 40.0f;
            Rectangle src = { 0.0f, 0.0f,
                              static_cast<float>(cards[i].icon->width),
                              static_cast<float>(cards[i].icon->height) };
            Rectangle dst = { static_cast<float>(cx) - kIconSize * 0.5f,
                              static_cast<float>(y0 + 22),
                              kIconSize, kIconSize };
            DrawTexturePro(*cards[i].icon, src, dst, {0.0f, 0.0f}, 0.0f,
                           Theme::withAlpha(WHITE, cards[i].vivid ? 255 : 150));
        }

        Fonts::drawCentered(Fonts::bold, cards[i].name, cx, y0 + 63,
                            15.0f, Theme::withAlpha(Theme::palette.son_bone, nameA));
        Fonts::drawCentered(Fonts::body, cards[i].hint, cx, y0 + 79,
                            11.0f, Theme::withAlpha(Theme::palette.son_bone, hintA));
    }
    return clicked;
}

void loadTextureIfPresent(Texture2D& tex, const char* path) {
    tex = {};
    if (!FileExists(path)) return;

    tex = LoadTexture(path);
    if (tex.id != 0) {
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
    }
}

void unloadTextureSafe(Texture2D& tex) {
    if (tex.id != 0) {
        UnloadTexture(tex);
        tex = {};
    }
}

void drawStoryIllustrationFrame(const Texture2D* tex, int xCenter, int yTop,
                                int frameW, int frameH) {
    Rectangle outer = {
        static_cast<float>(xCenter - frameW / 2),
        static_cast<float>(yTop),
        static_cast<float>(frameW),
        static_cast<float>(frameH)
    };
    DrawRectangleRounded(outer, 0.06f, 8,
                         Theme::withAlpha(Theme::palette.ink_sumi, 220));
    DrawRectangleRoundedLinesEx(outer, 0.06f, 8, 2.0f,
                                Theme::withAlpha(Theme::palette.gold_foil, 210));

    Rectangle inner = {
        outer.x + 8.0f, outer.y + 8.0f,
        outer.width - 16.0f, outer.height - 16.0f
    };
    DrawRectangleRounded(inner, 0.05f, 6,
                         Theme::withAlpha(Theme::palette.slate_fog, 235));

    if (!tex || tex->id == 0) return;

    float availW = inner.width - 10.0f;
    float availH = inner.height - 10.0f;
    float scale = std::min(availW / static_cast<float>(tex->width),
                           availH / static_cast<float>(tex->height));
    float drawW = static_cast<float>(tex->width) * scale;
    float drawH = static_cast<float>(tex->height) * scale;

    Rectangle src = { 0.0f, 0.0f,
                      static_cast<float>(tex->width),
                      static_cast<float>(tex->height) };
    Rectangle dst = {
        inner.x + (inner.width - drawW) * 0.5f,
        inner.y + (inner.height - drawH) * 0.5f,
        drawW,
        drawH
    };
    DrawTexturePro(*tex, src, dst, {0.0f, 0.0f}, 0.0f, WHITE);
}

}  // namespace

Game::Game()
    : state(GameState::Menu), settingsReturnState(GameState::Menu),
      player1(nullptr), player2(nullptr), currentPlayer(nullptr),
      cursorRow(Board::SIZE / 2), cursorCol(Board::SIZE / 2),
      vsAI(true), aiDepth(3),
      inStoryMode(false),
      storyMaxUnlocked(1), cheatUnlockAll(false),
      storySigilLastFillTime(-1.0f),
      storyPanelScroll(0.0f), storyPanelMaxScroll(0.0f),
      playTime(0.0f),
      toastMessage{}, toastTimer(0.0f),
      showDebugPanel(false),
      quitRequested(false),
      aiThinking(false), aiResult{-1, -1},
      networkMatchActive(false),
      localNetworkMark(CellState::Empty),
      waitingForNetworkAck(false),
      storyVoiIcon{}, storyGaIcon{}, storyNguaIcon{},
      storyIntroImages{},
      storySetIntroImages{}, storySetWinImages{}, storySetLoseImages{},
      storyUnlockImages{}, storyEpilogueImage{} {
    loadSettings();
}

Game::~Game() {
    if (aiThread.joinable()) aiThread.join();
    unloadStoryAssets();
    delete player1;
    delete player2;
}

void Game::run() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Caro Game - OOP1 Project");
    SetTargetFPS(60);
    SetExitKey(0);  // Disable ESC auto-close; we handle ESC ourselves

    renderer.init(SCREEN_WIDTH, SCREEN_HEIGHT);
    Fonts::init();
    audioManager.init();
    MenuScreen::preload();
    loadStoryAssets();

    while (!WindowShouldClose() && !quitRequested) {
        // Settings/Save/Load fall through on purpose: they keep whatever's
        // already playing so a quick visit doesn't cut the current track.
        // Story narration screens use menu music - they're between matches.
        switch (state) {
            case GameState::Menu:
            case GameState::Multiplayer:
            case GameState::StoryPickSet:
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
            case GameState::Multiplayer:    updateMultiplayer();    break;
            case GameState::Playing:        updatePlaying();        break;
            case GameState::GameOver:       updateGameOver();       break;
            case GameState::SaveScreen: // fallthrough
            case GameState::LoadScreen:     updateSaveLoadScreen(); break;
            case GameState::StoryPickSet:   updateStoryPickSet();   break;
            case GameState::StoryIntro:     updateStoryIntro();     break;
            case GameState::StoryBeat:      updateStoryBeat();      break;
        }

        if (quitRequested) break;

        // Draw
        BeginDrawing();
        ClearBackground(Theme::palette.sky_horizon);

        // 3-stop ink-wash sky behind the in-game 3D scene. Inline shader does
        // smoothstep-interpolation between sky_top / sky_mid / sky_horizon -
        // no Mach-band seam at the midline. Menu/Settings have their own
        // animated wuxia-storm BG, so only Playing/GameOver get the sky.
        if (state == GameState::Playing || state == GameState::GameOver) {
            renderer.drawSkyGradient();
        }

        switch (state) {
            case GameState::Menu:           drawMenu();           break;
            case GameState::Settings:       drawSettings();       break;
            case GameState::PickDifficulty: drawDifficulty();     break;
            case GameState::Multiplayer:    drawMultiplayer();    break;
            case GameState::Playing:        drawPlaying();        break;
            case GameState::GameOver:       drawGameOver();       break;
            case GameState::SaveScreen: // fallthrough
            case GameState::LoadScreen:     drawSaveLoadScreen(); break;
            case GameState::StoryPickSet:   drawStoryPickSet();   break;
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
    unloadStoryAssets();
    Fonts::cleanup();
    renderer.shutdown();
    MenuScreen::shutdown();
    CloseWindow();
}

void Game::loadStoryAssets() {
    loadTextureIfPresent(storyVoiIcon,
                         "assets/images/story/voi-chin-nga-hud-icon-wuxia-v1.png");
    if (storyVoiIcon.id == 0) {
    loadTextureIfPresent(storyVoiIcon,
                         "assets/images/story/voi-chin-nga-hud-icon-v1.png");
    }
    loadTextureIfPresent(storyGaIcon,
                         "assets/images/story/ga-chin-cua-hud-icon-wuxia-v1.png");
    if (storyGaIcon.id == 0) {
    loadTextureIfPresent(storyGaIcon,
                         "assets/images/story/ga-chin-cua-hud-icon-v1.png");
    }
    loadTextureIfPresent(storyNguaIcon,
                         "assets/images/story/ngua-chin-hong-mao-hud-icon-wuxia-v1.png");
    if (storyNguaIcon.id == 0) {
    loadTextureIfPresent(storyNguaIcon,
                         "assets/images/story/ngua-chin-hong-mao-hud-icon-v1.png");
    }

    char path[128];
    for (int i = 0; i < StoryContent::kIntroPageCount; ++i) {
        std::snprintf(path, sizeof(path),
                      "assets/images/story/intro/intro-page-%02d-wuxia-v4.png", i + 1);
        loadTextureIfPresent(storyIntroImages[i], path);
        if (storyIntroImages[i].id == 0) {
            std::snprintf(path, sizeof(path),
                      "assets/images/story/intro/intro-page-%02d-wuxia-v3.png", i + 1);
            loadTextureIfPresent(storyIntroImages[i], path);
        }
        if (storyIntroImages[i].id == 0) {
            std::snprintf(path, sizeof(path),
                      "assets/images/story/intro/intro-page-%02d-v2.png", i + 1);
            loadTextureIfPresent(storyIntroImages[i], path);
        }
        if (storyIntroImages[i].id == 0) {
            std::snprintf(path, sizeof(path),
                          "assets/images/story/intro/intro-page-%02d.png", i + 1);
            loadTextureIfPresent(storyIntroImages[i], path);
        }
    }

    const char* const setNames[4] = { "set1", "set2", "set3", "finalboss" };
    for (int i = 0; i < 4; ++i) {
        std::snprintf(path, sizeof(path),
                      "assets/images/story/set-intro/%s-wuxia-v1.png", setNames[i]);
        loadTextureIfPresent(storySetIntroImages[i], path);
        if (storySetIntroImages[i].id == 0) {
            std::snprintf(path, sizeof(path),
                          "assets/images/story/set-intro/%s-v1.png", setNames[i]);
            loadTextureIfPresent(storySetIntroImages[i], path);
        }

        std::snprintf(path, sizeof(path),
                      "assets/images/story/set-win/%s-wuxia-v1.png", setNames[i]);
        loadTextureIfPresent(storySetWinImages[i], path);
        if (storySetWinImages[i].id == 0) {
            std::snprintf(path, sizeof(path),
                          "assets/images/story/set-win/%s-v1.png", setNames[i]);
            loadTextureIfPresent(storySetWinImages[i], path);
        }

        std::snprintf(path, sizeof(path),
                      "assets/images/story/set-lose/%s-wuxia-v1.png", setNames[i]);
        loadTextureIfPresent(storySetLoseImages[i], path);
        if (storySetLoseImages[i].id == 0) {
            std::snprintf(path, sizeof(path),
                          "assets/images/story/set-lose/%s-v1.png", setNames[i]);
            loadTextureIfPresent(storySetLoseImages[i], path);
        }
    }

    const char* const unlockNames[3] = { "voi", "ga", "ngua" };
    for (int i = 0; i < 3; ++i) {
        std::snprintf(path, sizeof(path),
                      "assets/images/story/unlock/%s-wuxia-v1.png", unlockNames[i]);
        loadTextureIfPresent(storyUnlockImages[i], path);
        if (storyUnlockImages[i].id == 0) {
            std::snprintf(path, sizeof(path),
                          "assets/images/story/unlock/%s-v1.png", unlockNames[i]);
            loadTextureIfPresent(storyUnlockImages[i], path);
        }
    }

    loadTextureIfPresent(storyEpilogueImage,
                         "assets/images/story/epilogue/epilogue-wuxia-v1.png");
}

void Game::unloadStoryAssets() {
    unloadTextureSafe(storyVoiIcon);
    unloadTextureSafe(storyGaIcon);
    unloadTextureSafe(storyNguaIcon);
    for (Texture2D& tex : storyIntroImages) {
        unloadTextureSafe(tex);
    }
    for (Texture2D& tex : storySetIntroImages) {
        unloadTextureSafe(tex);
    }
    for (Texture2D& tex : storySetWinImages) {
        unloadTextureSafe(tex);
    }
    for (Texture2D& tex : storySetLoseImages) {
        unloadTextureSafe(tex);
    }
    for (Texture2D& tex : storyUnlockImages) {
        unloadTextureSafe(tex);
    }
    unloadTextureSafe(storyEpilogueImage);
}

void Game::updateMenu() {
    if (toastTimer > 0.0f) toastTimer -= GetFrameTime();

    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        quitRequested = true;
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
            // Picker is the entry hub - read storyMaxUnlocked + cheat flag,
            // then user picks which set to play. Reset story state on the
            // picker click, not here, so backing out (ESC → Menu) doesn't
            // leak partial state.
            state = GameState::StoryPickSet;
            menuScreen.reset();
            break;
        case MenuChoice::LoadGame:
            saveLoadScreen.open(SlotScreenMode::Load);
            state = GameState::LoadScreen;
            menuScreen.reset();
            break;
        case MenuChoice::Multiplayer:
            multiplayerScreen.reset();
            state = GameState::Multiplayer;
            menuScreen.reset();
            break;
        case MenuChoice::Settings:
            settingsScreen.setSettings({vsAI, cheatUnlockAll});
            settingsScreen.reset();
            settingsReturnState = GameState::Menu;
            state = GameState::Settings;
            menuScreen.reset();
            break;
        case MenuChoice::Exit:
            quitRequested = true;
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
        cheatUnlockAll = s.cheatUnlockAll;
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

void Game::updateMultiplayer() {
    handleNetworkEvents();

    multiplayerScreen.update(audioManager);
    MultiplayerAction action = multiplayerScreen.consumeAction();
    switch (action.type) {
        case MultiplayerActionType::BackToMenu:
            networkSession.shutdown();
            multiplayerScreen.reset();
            state = GameState::Menu;
            menuScreen.reset();
            break;
        case MultiplayerActionType::CancelWaiting:
            networkSession.shutdown();
            multiplayerScreen.reset();
            break;
        case MultiplayerActionType::StartLanHost:
            if (!networkSession.startLanHost(action.port, action.playerName)) {
                multiplayerScreen.setStatusMessage("Failed to host LAN game");
                break;
            }
            multiplayerScreen.setWaitingView("HOST LAN GAME",
                                             "Waiting for LAN player...", "");
            break;
        case MultiplayerActionType::StartLanJoin:
            if (!networkSession.startLanJoin(action.address, action.port,
                                             action.playerName)) {
                multiplayerScreen.setStatusMessage("Failed to join LAN host");
                break;
            }
            multiplayerScreen.setWaitingView("JOIN LAN GAME",
                                             "Connecting to host...", "");
            break;
        case MultiplayerActionType::StartOnlineHost:
            {
                ServerConfig::Endpoint endpoint =
                    ServerConfig::resolveOnlineEndpoint(action.endpointPreset);
                if (!ServerConfig::endpointConfigured(endpoint)) {
                    multiplayerScreen.setStatusMessage(
                        "Online server not configured",
                        "Set CARO_SERVER_HOST or build with CARO_PRODUCTION_SERVER_HOST");
                    break;
                }
                if (!networkSession.startOnlineHost(endpoint.host, endpoint.port,
                                                action.playerName)) {
                    multiplayerScreen.setStatusMessage("Failed to reach online server");
                    break;
                }
                multiplayerScreen.setWaitingView(
                    "ONLINE ROOM", "Creating room...",
                    ServerConfig::advancedSelectorEnabled()
                        ? ServerConfig::presetSummary(action.endpointPreset)
                        : "");
            }
            break;
        case MultiplayerActionType::StartOnlineJoin:
            {
                ServerConfig::Endpoint endpoint =
                    ServerConfig::resolveOnlineEndpoint(action.endpointPreset);
                if (!ServerConfig::endpointConfigured(endpoint)) {
                    multiplayerScreen.setStatusMessage(
                        "Online server not configured",
                        "Set CARO_SERVER_HOST or build with CARO_PRODUCTION_SERVER_HOST");
                    break;
                }
                if (!networkSession.startOnlineJoin(endpoint.host, endpoint.port,
                                                    action.roomCode, action.playerName)) {
                    multiplayerScreen.setStatusMessage("Failed to join online room");
                    break;
                }
                multiplayerScreen.setWaitingView(
                    "ONLINE ROOM", "Joining room...",
                    ServerConfig::advancedSelectorEnabled()
                        ? ServerConfig::presetSummary(action.endpointPreset)
                        : "");
            }
            break;
        case MultiplayerActionType::None:
            break;
    }
}

void Game::updatePlaying() {
    playTime += GetFrameTime();
    if (toastTimer > 0.0f) toastTimer -= GetFrameTime();

    if (renderer.updateCamera()) {
        audioManager.playMenuClickSound();
    }
    renderer.updateParticles(GetFrameTime());
    if (networkMatchActive) handleNetworkEvents();

    // Check if AI finished thinking
    if (aiThinking.load() == false && aiResult.row >= 0) {
        applyMove(aiResult);
        aiResult = {-1, -1};
        return;
    }

    // If AI is thinking, skip input but keep rendering
    if (aiThinking.load()) return;

    handleInput();
    if (state != GameState::Playing) return;

    // If current player is AI, spawn thread
    if (!networkMatchActive && dynamic_cast<AIPlayer*>(currentPlayer) != nullptr) {
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

    if (networkMatchActive) {
        handleNetworkEvents();
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
            audioManager.playMenuClickSound();
            leaveNetworkSession();
        }
        return;
    }

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

void Game::drawMultiplayer() {
    multiplayerScreen.draw();
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

    if (networkMatchActive) {
        const char* banner = waitingForNetworkAck ? "Syncing move..."
            : ((currentPlayer->getMark() == localNetworkMark)
                   ? "Your turn"
                   : "Opponent's turn");
        Fonts::draw(Fonts::body, banner, GetScreenWidth() - 198, 232, 14.0f,
                    Theme::palette.gold_foil);
    }

    // Camera control buttons (2D overlay)
    renderer.drawCameraControls();

    // AI thinking indicator - below HUD panel
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
    if (!networkMatchActive && renderer.drawSaveButton()) {
        audioManager.playMenuClickSound();
        saveLoadScreen.open(SlotScreenMode::Save);
        state = GameState::SaveScreen;
    }
    if (!networkMatchActive && renderer.drawLoadButton()) {
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
        if (networkMatchActive) {
            leaveNetworkSession("Left multiplayer match");
        } else {
            menuScreen.reset();
            state = GameState::Menu;
        }
    }
    if (renderer.drawSettingsButton()) {
        audioManager.playMenuClickSound();
        settingsScreen.setSettings({vsAI, cheatUnlockAll});
        settingsScreen.reset();
        settingsReturnState = GameState::Playing;
        state = GameState::Settings;
    }

    // Undo button - hidden in Story Mode (Voi card is the only undo path).
    if (!networkMatchActive && !inStoryMode && renderer.drawUndoButton()) {
        audioManager.playMenuClickSound();
        undoLastMove();
    }

    // Restart button
    if (!networkMatchActive && renderer.drawRestartButton()) {
        audioManager.playMenuClickSound();
        startNewGame();
    }

    // Story-mode overlays - linh vật charges + set score
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
        // Story Mode: skip the generic banner - the sigil already announces
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
        std::snprintf(msg, sizeof(msg), "%s wins! %s", winnerName,
                      networkMatchActive ? "Enter=Menu, ESC=Menu"
                                         : "Enter=New Game, ESC=Menu");
        gameScreen.drawMessage(msg);
    } else {
        gameScreen.drawMessage(networkMatchActive
                                   ? "Draw! Enter=Menu, ESC=Menu"
                                   : "Draw! Enter=New Game, ESC=Menu");
    }
}

void Game::startNetworkMatch(const NetEvent& event) {
    audioManager.stopGameOverSounds();
    if (aiThread.joinable()) aiThread.join();
    aiThinking.store(false);
    aiResult = {-1, -1};

    inStoryMode = false;
    networkMatchActive = true;
    localNetworkMark = event.mark;
    waitingForNetworkAck = false;

    delete player1;
    delete player2;
    player1 = new Player(event.player1Name.empty() ? "Player 1"
                                                   : event.player1Name,
                         CellState::PlayerX);
    player2 = new Player(event.player2Name.empty() ? "Player 2"
                                                   : event.player2Name,
                         CellState::PlayerO);

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

void Game::leaveNetworkSession(const char* toast) {
    networkSession.shutdown();
    networkMatchActive = false;
    localNetworkMark = CellState::Empty;
    waitingForNetworkAck = false;
    if (toast && toast[0] != '\0') {
        std::snprintf(toastMessage, sizeof(toastMessage), "%s", toast);
        toastTimer = 2.5f;
    }
    menuScreen.reset();
    state = GameState::Menu;
}

bool Game::canLocalHumanMove() const {
    if (currentPlayer == nullptr) return false;
    if (dynamic_cast<AIPlayer*>(currentPlayer) != nullptr) return false;
    if (networkMatchActive) {
        if (currentPlayer->getMark() != localNetworkMark) return false;
        if (waitingForNetworkAck) return false;
    }
    return true;
}

void Game::submitHumanMove(Move move) {
    if (!canLocalHumanMove()) return;

    if (!networkMatchActive) {
        applyMove(move);
        return;
    }

    if (networkSession.localAppliesMoves()) {
        int before = board.getMoveCount();
        CellState mark = currentPlayer->getMark();
        applyMove(move);
        if (board.getMoveCount() == before + 1) {
            networkSession.sendAppliedMove(mark, move);
        }
        return;
    }

    waitingForNetworkAck = true;
    networkSession.sendMoveRequest(move);
}

void Game::handleNetworkEvents() {
    std::vector<NetEvent> events = networkSession.pollEvents();
    for (const NetEvent& event : events) {
        switch (event.type) {
            case NetEventType::RoomCode:
                multiplayerScreen.setRoomCode(event.roomCode);
                multiplayerScreen.setStatusMessage("Waiting for online opponent...",
                                                   "Room code: " + event.roomCode);
                break;
            case NetEventType::Waiting:
                multiplayerScreen.setStatusMessage(event.text);
                break;
            case NetEventType::Info:
                if (state == GameState::Multiplayer) {
                    multiplayerScreen.setStatusMessage(event.text);
                } else {
                    std::snprintf(toastMessage, sizeof(toastMessage), "%s",
                                  event.text.c_str());
                    toastTimer = 2.0f;
                }
                break;
            case NetEventType::MatchStarted:
                startNetworkMatch(event);
                break;
            case NetEventType::MoveRequest:
                if (!networkMatchActive || !networkSession.localAppliesMoves()) break;
                if (state != GameState::Playing) break;
                if (currentPlayer == nullptr ||
                    currentPlayer->getMark() != event.mark ||
                    !board.isEmpty(event.move.row, event.move.col)) {
                    break;
                }
                {
                    int before = board.getMoveCount();
                    applyMove(event.move);
                    if (board.getMoveCount() == before + 1) {
                        networkSession.sendAppliedMove(event.mark, event.move);
                    }
                }
                break;
            case NetEventType::MoveApplied:
                if (!networkMatchActive) break;
                if (state != GameState::Playing) break;
                if (currentPlayer == nullptr ||
                    currentPlayer->getMark() != event.mark ||
                    !board.isEmpty(event.move.row, event.move.col)) {
                    std::snprintf(toastMessage, sizeof(toastMessage),
                                  "Network move desync detected");
                    toastTimer = 2.5f;
                    break;
                }
                if (event.mark == localNetworkMark) {
                    waitingForNetworkAck = false;
                }
                applyMove(event.move);
                break;
            case NetEventType::PeerDisconnected:
                leaveNetworkSession(event.text.c_str());
                break;
            case NetEventType::Error:
                if (state == GameState::Multiplayer) {
                    networkSession.shutdown();
                    multiplayerScreen.setStatusMessage("Network error", event.text);
                } else {
                    leaveNetworkSession(event.text.c_str());
                }
                break;
        }
    }
}

void Game::startNewGame() {
    audioManager.stopGameOverSounds();

    if (aiThread.joinable()) aiThread.join();
    aiThinking.store(false);
    aiResult = {-1, -1};
    networkMatchActive = false;
    localNetworkMark = CellState::Empty;
    waitingForNetworkAck = false;

    if (inStoryMode) {
        storyMode.onMatchStart();
        // New match starts fresh - drop the stale pulse/wash/caption from
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
    resetStoryPanelScroll();
    startNewGame();
}

void Game::exitStoryToMenu() {
    inStoryMode = false;
    storyMode.reset();
    resetStoryPanelScroll();
    state = GameState::Menu;
}

void Game::resetStoryPanelScroll() {
    storyPanelScroll = 0.0f;
    storyPanelMaxScroll = 0.0f;
}

void Game::updateStoryPanelScroll() {
    float next = storyPanelScroll;
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) next -= wheel * kStoryPanelScrollStep;
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) next -= kStoryPanelLineStep;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) next += kStoryPanelLineStep;
    if (IsKeyPressed(KEY_PAGE_UP)) next -= kStoryPanelScrollStep * 2.0f;
    if (IsKeyPressed(KEY_PAGE_DOWN)) next += kStoryPanelScrollStep * 2.0f;
    if (IsKeyPressed(KEY_HOME)) next = 0.0f;
    if (IsKeyPressed(KEY_END)) next = storyPanelMaxScroll;
    storyPanelScroll = std::max(0.0f, std::min(next, storyPanelMaxScroll));
}

void Game::switchTurn() {
    currentPlayer = (currentPlayer == player1) ? player2 : player1;
}

bool Game::isCurrentMatchVsAI() const {
    return dynamic_cast<AIPlayer*>(player2) != nullptr;
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
            if (board.isEmpty(row, col) && canLocalHumanMove()) {
                submitHumanMove({row, col});
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
        if (canLocalHumanMove() && board.isEmpty(cursorRow, cursorCol)) {
            submitHumanMove({cursorRow, cursorCol});
        }
    }

    // ESC → return to menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        if (aiThread.joinable()) aiThread.join();
        aiThinking.store(false);
        aiResult = {-1, -1};
        if (networkMatchActive) {
            leaveNetworkSession("Left multiplayer match");
        } else {
            menuScreen.reset();
            state = GameState::Menu;
        }
        return;
    }

    // Debug panel toggle
    if (IsKeyPressed(KEY_F3)) {
        showDebugPanel = !showDebugPanel;
    }

    // Save/Load shortcuts (Ctrl+S / Ctrl+L)
    if (!networkMatchActive &&
        (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
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
        // 1 = Voi 9 ngà - undo 5 player turns (10 board moves in PvAI).
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

int storySetIndex(StoryMode::SetId id) {
    return static_cast<int>(id);
}

int storyUnlockIndex(StoryMode::SetId id) {
    switch (id) {
        case StoryMode::SetId::Set1: return 0;
        case StoryMode::SetId::Set2: return 1;
        case StoryMode::SetId::Set3: return 2;
        case StoryMode::SetId::FinalBoss: return -1;
    }
    return -1;
}

}  // namespace

void Game::updateStoryPickSet() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        state = GameState::Menu;
        menuScreen.reset();
    }
}

void Game::drawStoryPickSet() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, Theme::palette.ink_sumi);
    UIC::drawTitle("BẢNG CHỌN ẢI", sw, sh, sh / 9);

    Fonts::drawCentered(Fonts::body,
                        "Chọn chương để vào trận. Linh vật làm mới mỗi ải.",
                        sw / 2, sh / 9 + 64, 14.0f,
                        Theme::withAlpha(Theme::palette.son_bone, 200));

    int maxAvail = cheatUnlockAll ? 4 : storyMaxUnlocked;

    struct CardMeta {
        const char* tag;
        const char* title;
        const char* beasts;
    };
    static const CardMeta meta[4] = {
        { "SET 1 · DỄ",    "MƯA GIÔNG",          "Chưa có linh vật" },
        { "SET 2 · VỪA",   "CUỒNG PHONG",        "Voi ×1" },
        { "SET 3 · KHÓ",   "CƠN BÃO TẬN THẾ",    "Voi ×1, Gà ×3" },
        { "BOSS · TỬ ĐẤU", "CHÂN HÌNH THUỶ TINH","Đầy đủ 3 linh vật" },
    };

    constexpr int kCardW = 168;
    constexpr int kCardH = 220;
    constexpr int kGap   = 22;
    int totalW = 4 * kCardW + 3 * kGap;
    int leftX  = (sw - totalW) / 2;
    int topY   = sh / 2 - kCardH / 2 + 10;

    Vector2 mp = GetMousePosition();
    for (int i = 0; i < 4; ++i) {
        int x0 = leftX + i * (kCardW + kGap);
        int y0 = topY;
        int cx = x0 + kCardW / 2;
        bool unlocked = (i + 1) <= maxAvail;
        Rectangle r = { static_cast<float>(x0), static_cast<float>(y0),
                        static_cast<float>(kCardW), static_cast<float>(kCardH) };
        bool hot = unlocked && CheckCollisionPointRec(mp, r);

        if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            audioManager.playMenuClickSound();
            inStoryMode = true;
            storyMode.jumpToSet(static_cast<StoryMode::SetId>(i));
            storySigilLastFillTime = -1.0f;
            resetStoryPanelScroll();
            state = GameState::StoryIntro;
            return;
        }

        unsigned char bgA     = unlocked ? 215 : 130;
        unsigned char borderA = unlocked ? (hot ? 250 : 180) : 80;
        DrawRectangleRounded(r, 0.10f, 8,
                             Theme::withAlpha(Theme::palette.ink_sumi, bgA));
        DrawRectangleRoundedLinesEx(r, 0.10f, 8, 2.5f,
                                    Theme::withAlpha(Theme::palette.gold_foil, borderA));

        unsigned char tagA   = unlocked ? 240 : 130;
        unsigned char titleA = unlocked ? 245 : 140;

        Fonts::drawCentered(Fonts::bold, meta[i].tag, cx, y0 + 18, 13.0f,
                            Theme::withAlpha(Theme::palette.gold_foil, tagA));
        Fonts::drawCentered(Fonts::bold, meta[i].title, cx, y0 + 56, 17.0f,
                            Theme::withAlpha(Theme::palette.son_bone, titleA));

        if (unlocked) {
            Fonts::drawCentered(Fonts::body, meta[i].beasts, cx, y0 + 110, 13.0f,
                                Theme::withAlpha(Theme::palette.son_bone, 220));
            Fonts::drawCentered(Fonts::bold, "Vào trận",
                                cx, y0 + kCardH - 38, 15.0f,
                                Theme::withAlpha(Theme::palette.gold_foil,
                                                 hot ? 255 : 200));
        } else {
            Fonts::drawCentered(Fonts::bold, "[ KHÓA ]",
                                cx, y0 + kCardH / 2 + 4, 16.0f,
                                Theme::withAlpha(Theme::palette.son_bone, 150));
            Fonts::drawCentered(Fonts::body, "Thắng ải trước để mở",
                                cx, y0 + kCardH / 2 + 36, 12.0f,
                                Theme::withAlpha(Theme::palette.son_bone, 110));
        }
    }

    int backX = sw / 2 - 110;
    int backY = sh - 80;
    Rectangle backBtn = { static_cast<float>(backX),
                          static_cast<float>(backY), 220.0f, 44.0f };
    UIC::State bs = UIC::State::Rest;
    if (CheckCollisionPointRec(mp, backBtn)) {
        bs = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? UIC::State::Pressed
                                                  : UIC::State::Focused;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            audioManager.playMenuClickSound();
            state = GameState::Menu;
            menuScreen.reset();
        }
    }
    UIC::drawPrimaryButton(backBtn, "Quay lại", bs);
}

void Game::updateStoryIntro() {
    updateStoryPanelScroll();

    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        exitStoryToMenu();
        return;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        audioManager.playMenuClickSound();
        storyMode.advance();
        resetStoryPanelScroll();
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

    int panelY = 110;
    const Texture2D* introArt = nullptr;
    if (storyMode.introPageIdx >= 0
        && storyMode.introPageIdx < StoryContent::kIntroPageCount
        && storyIntroImages[storyMode.introPageIdx].id != 0) {
        introArt = &storyIntroImages[storyMode.introPageIdx];
    }
    if (introArt) {
        drawStoryIllustrationFrame(introArt, w / 2, 92, 540, 210);
        panelY = 318;
    }

    UIC::ComicPanel cp = {
        "MỞ ĐẦU",
        tag,
        nullptr,
        StoryContent::kIntroPages[storyMode.introPageIdx],
        kStoryPanelWidthCh
    };
    float panelMaxScroll = 0.0f;
    UIC::drawComicPanel(cp, w / 2, panelY,
                        storyPanelMaxHeightFor(panelY, h),
                        storyPanelScroll, &panelMaxScroll);
    storyPanelMaxScroll = panelMaxScroll;
    if (storyPanelScroll > storyPanelMaxScroll) {
        storyPanelScroll = storyPanelMaxScroll;
    }

    NavBtn btns[3] = {
        { "Prev", storyMode.introPageIdx > 0 },
        { "Skip", true },
        { "Next", true },
    };
    int clicked = drawNavRow(w, static_cast<int>(storyNavRowCenterY(h)), btns, 3);
    if (clicked == 0) {
        audioManager.playMenuClickSound();
        if (storyMode.introPageIdx > 0) {
            --storyMode.introPageIdx;
            resetStoryPanelScroll();
        }
    } else if (clicked == 1) {
        // Skip: jump straight into Set1 match. Chain advance() twice -
        // IntroMonologue→SetIntro then SetIntro→MatchPlaying - and start
        // the match immediately so the player skips ALL narrative panels.
        audioManager.playMenuClickSound();
        storyMode.introPageIdx = StoryContent::kIntroPageCount - 1;
        resetStoryPanelScroll();
        storyMode.advance();   // → SetIntro
        storyMode.advance();   // → MatchPlaying
        startStoryMatch();
    } else if (clicked == 2) {
        audioManager.playMenuClickSound();
        storyMode.advance();
        resetStoryPanelScroll();
        if (storyMode.subBeat == StoryMode::SubBeat::SetIntro) {
            state = GameState::StoryBeat;
        }
    }

    UIC::drawHintBar("W/S cuộn · ENTER tiếp · ESC về menu", w, h);
}

void Game::updateStoryBeat() {
    updateStoryPanelScroll();

    if (IsKeyPressed(KEY_ESCAPE)) {
        audioManager.playMenuClickSound();
        exitStoryToMenu();
        return;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        audioManager.playMenuClickSound();

        // Epilogue is a leaf - Enter exits to Menu without advancing.
        if (storyMode.subBeat == StoryMode::SubBeat::Epilogue) {
            exitStoryToMenu();
            return;
        }

        StoryMode::SubBeat oldBeat = storyMode.subBeat;
        StoryMode::SetId   oldSet  = storyMode.currentSet;
        storyMode.advance();
        resetStoryPanelScroll();
        if (oldBeat == StoryMode::SubBeat::SetWin) {
            onSetCompleted(oldSet);
        }

        // After advance: route GameState by the new subBeat.
        if (storyMode.subBeat == StoryMode::SubBeat::MatchPlaying) {
            startStoryMatch();
        }
        // Otherwise stay on StoryBeat - next render shows the new panel.
    }
}

void Game::drawStoryBeat() {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    DrawRectangleGradientV(0, 0, w, h,
                           Theme::palette.bg_top, Theme::palette.bg_bottom);

    const char* hint = "W/S cuộn · ENTER tiếp · ESC về menu";
    UIC::ComicPanel cp = { nullptr, nullptr, nullptr, "", kStoryPanelWidthCh };
    const Texture2D* beatArt = nullptr;
    int panelY = 110;

    switch (storyMode.subBeat) {
        case StoryMode::SubBeat::SetIntro: {
            const auto& st = setTextFor(storyMode.currentSet);
            UIC::drawTitle(st.title, w, h, 30);
            cp.title = "VÀO TRẬN";
            cp.tag   = st.tag;
            cp.body  = st.intro;
            int setIdx = storySetIndex(storyMode.currentSet);
            if (setIdx >= 0 && setIdx < static_cast<int>(storySetIntroImages.size())
                && storySetIntroImages[setIdx].id != 0) {
                beatArt = &storySetIntroImages[setIdx];
            }
            // Final boss reveals the kraken art in the intro.
            if (!beatArt && storyMode.currentSet == StoryMode::SetId::FinalBoss) {
                cp.plot = StoryContent::kThuyTinhBossArt;
            }
            hint = "W/S cuộn · ENTER bắt đầu trận · ESC về menu";
            break;
        }
        case StoryMode::SubBeat::SetWin: {
            const auto& st = setTextFor(storyMode.currentSet);
            UIC::drawTitle(st.title, w, h, 30);
            cp.title = "THẮNG";
            cp.tag   = st.tag;
            cp.body  = st.win;
            int setIdx = storySetIndex(storyMode.currentSet);
            if (setIdx >= 0 && setIdx < static_cast<int>(storySetWinImages.size())
                && storySetWinImages[setIdx].id != 0) {
                beatArt = &storySetWinImages[setIdx];
            }
            hint = "W/S cuộn · ENTER tiếp";
            break;
        }
        case StoryMode::SubBeat::SetLose: {
            const auto& st = setTextFor(storyMode.currentSet);
            UIC::drawTitle(st.title, w, h, 30);
            cp.title = "BẠI";
            cp.tag   = st.tag;
            cp.body  = st.lose;
            int setIdx = storySetIndex(storyMode.currentSet);
            if (setIdx >= 0 && setIdx < static_cast<int>(storySetLoseImages.size())
                && storySetLoseImages[setIdx].id != 0) {
                beatArt = &storySetLoseImages[setIdx];
            }
            hint = "W/S cuộn · ENTER chơi lại set";
            break;
        }
        case StoryMode::SubBeat::LinhVatUnlock: {
            UIC::drawTitle("LINH VẬT", w, h, 30);
            cp.title = "BAN THƯỞNG";
            cp.tag   = linhVatNameFor(storyMode.currentSet);
            cp.body  = linhVatUnlockLineFor(storyMode.currentSet);
            int unlockIdx = storyUnlockIndex(storyMode.currentSet);
            if (unlockIdx >= 0
                && unlockIdx < static_cast<int>(storyUnlockImages.size())
                && storyUnlockImages[unlockIdx].id != 0) {
                beatArt = &storyUnlockImages[unlockIdx];
            }
            hint = "W/S cuộn · ENTER tiếp · ESC về menu";
            break;
        }
        case StoryMode::SubBeat::Epilogue: {
            UIC::drawTitle("HỒI KẾT", w, h, 30);
            cp.title = "CÔ SỬ TIÊN";
            cp.tag   = "VĨ THANH";
            cp.body  = StoryContent::kEpilogueLine;
            if (storyEpilogueImage.id != 0) {
                beatArt = &storyEpilogueImage;
            }
            hint = "W/S cuộn · ENTER về menu";
            break;
        }
        default:
            // MatchPlaying / IntroMonologue should not reach drawStoryBeat.
            break;
    }

    if (beatArt) {
        drawStoryIllustrationFrame(beatArt, w / 2, 92, 540, 210);
        panelY = 318;
    }

    float panelMaxScroll = 0.0f;
    UIC::drawComicPanel(cp, w / 2, panelY,
                        storyPanelMaxHeightFor(panelY, h),
                        storyPanelScroll, &panelMaxScroll);
    storyPanelMaxScroll = panelMaxScroll;
    if (storyPanelScroll > storyPanelMaxScroll) {
        storyPanelScroll = storyPanelMaxScroll;
    }

    // Epilogue is a leaf - only Menu is shown; all other beats add Next.
    int btnCount = (storyMode.subBeat == StoryMode::SubBeat::Epilogue) ? 1 : 2;
    NavBtn btns[2] = { { "Menu", true }, { "Next", true } };
    int clicked = drawNavRow(w, static_cast<int>(storyNavRowCenterY(h)), btns, btnCount);
    if (clicked == 0) {
        audioManager.playMenuClickSound();
        exitStoryToMenu();
    } else if (clicked == 1) {
        audioManager.playMenuClickSound();
        StoryMode::SubBeat oldBeat = storyMode.subBeat;
        StoryMode::SetId   oldSet  = storyMode.currentSet;
        storyMode.advance();
        resetStoryPanelScroll();
        if (oldBeat == StoryMode::SubBeat::SetWin) {
            onSetCompleted(oldSet);
        }
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

    // TT stats - hit rate = ttHits / ttProbes, cutoff rate = ttCutoffs / ttHits
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

    // Bottom-right: linh vật cards. Voi/Gà clickable when charges > 0;
    // Ngựa is auto-on-loss (informational only). Camera buttons live
    // bottom-left (Renderer), so the right corner is free for these.
    int voiC  = storyMode.chargesLeft(StoryMode::LinhVat::Voi);
    int gaC   = storyMode.chargesLeft(StoryMode::LinhVat::Ga);
    int nguaC = storyMode.chargesLeft(StoryMode::LinhVat::Ngua);

    // Locked / spent are shared text; Voi & Gà show "×N" for live charges,
    // Ngựa just shows "Sẵn sàng" since it auto-fires (count is always 1).
    auto chargeStr = [](int c, const char* readyText, char* buf, size_t n) -> const char* {
        if (c < 0)  return "Khóa";
        if (c == 0) return "Hết";
        if (readyText) return readyText;
        std::snprintf(buf, n, "×%d", c);
        return buf;
    };
    char voiBuf[8], gaBuf[8];

    BeastCard cards[3] = {
        { "1", "VOI",  "Hoàn 5 nước",
          chargeStr(voiC, nullptr, voiBuf, sizeof(voiBuf)),
          &storyVoiIcon,
          voiC > 0, voiC > 0 },
        { "2", "GÀ",   "AI loạn 1 lượt",
          chargeStr(gaC, nullptr, gaBuf, sizeof(gaBuf)),
          &storyGaIcon,
          gaC > 0, gaC > 0 },
        { "3", "NGỰA", "Hồi sinh tự động",
          chargeStr(nguaC, "Sẵn sàng", nullptr, 0),
          &storyNguaIcon,
          false, nguaC > 0 },
    };

    bool interactive = (state == GameState::Playing) && !aiThinking.load();
    int clicked = drawBeastCardRow(GetScreenWidth() - 16,
                                   GetScreenHeight() - 16,
                                   cards, interactive);

    if (clicked == 0 && storyMode.useVoi()) {
        audioManager.playMenuClickSound();
        undoTurns(5);
        std::snprintf(toastMessage, sizeof(toastMessage),
                      "Voi 9 ngà - hoàn tác 5 nước cờ");
        toastTimer = 2.5f;
    } else if (clicked == 1 && storyMode.useGa()) {
        audioManager.playMenuClickSound();
        std::snprintf(toastMessage, sizeof(toastMessage),
                      "Gà 9 cựa gáy! Đối thủ loạn trí 1 lượt");
        toastTimer = 2.5f;
    }

    // Middle-bottom: tam-thai sigil. Three orbs track best-of-3 outcomes;
    // pulses + radial screen-wash + caption all key off storySigilLastFillTime
    // captured at onMatchEnd.
    StorySigil::Layout sigil{};
    sigil.centerX = GetScreenWidth() / 2;
    sigil.bottomY = GetScreenHeight() - 12;
    auto now = static_cast<float>(GetTime());

    // Wash and caption durations from StorySigil.cpp's anon namespace -
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

    SaveGameMode mode = inStoryMode ? SaveGameMode::Story
                      : (isCurrentMatchVsAI() ? SaveGameMode::PvAI
                                              : SaveGameMode::PvP);
    data.header.gameMode = static_cast<int>(mode);
    data.header.aiDepth = aiDepth;
    data.header.currentTurn = (currentPlayer == player1) ? 1 : 2;
    data.header.p1Wins = player1->getWins();
    data.header.p2Wins = player2->getWins();
    data.header.p1Moves = player1->getMovesMade();
    data.header.p2Moves = player2->getMovesMade();
    std::strncpy(data.header.p1Name, player1->getName().c_str(), 31);
    std::strncpy(data.header.p2Name, player2->getName().c_str(), 31);

    if (inStoryMode) {
        data.header.storySetId        = static_cast<int>(storyMode.currentSet);
        data.header.storyMatchWins    = storyMode.matchWinsInSet;
        data.header.storyMatchLosses  = storyMode.matchLossesInSet;
        data.header.storyVoiCharges   = storyMode.voiCharges;
        data.header.storyGaCharges    = storyMode.gaCharges;
        data.header.storyNguaCharges  = storyMode.nguaCharges;
    } else {
        data.header.storySetId        = -1;
        data.header.storyMatchWins    = 0;
        data.header.storyMatchLosses  = 0;
        data.header.storyVoiCharges   = -1;
        data.header.storyGaCharges    = -1;
        data.header.storyNguaCharges  = -1;
    }

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

    auto mode = static_cast<SaveGameMode>(data.header.gameMode);
    vsAI        = (mode == SaveGameMode::PvAI || mode == SaveGameMode::Story);
    inStoryMode = (mode == SaveGameMode::Story);
    aiDepth     = data.header.aiDepth;
    playTime    = data.header.playTime;

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

    if (inStoryMode) {
        storyMode.restore(static_cast<StoryMode::SetId>(data.header.storySetId),
                          data.header.storyMatchWins,
                          data.header.storyMatchLosses,
                          data.header.storyVoiCharges,
                          data.header.storyGaCharges,
                          data.header.storyNguaCharges);
        storySigilLastFillTime = -1.0f;
    } else {
        storyMode.reset();
    }

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
            const bool aiWon      = hasWinner && isCurrentMatchVsAI() &&
                                    currentPlayer == player2;
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
    int undoCount = (isCurrentMatchVsAI() && moveHistory.size() >= 2) ? 2 : 1;

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
    if (player1 == nullptr || networkMatchActive) return;
    SaveData data{};
    buildSaveData(data);
    FileManager::saveSlot(0, data);
}

void Game::saveSettings() const {
    FILE* f = fopen("settings.cfg", "w");
    if (!f) return;
    fprintf(f, "%d\n", vsAI ? 1 : 0);
    fprintf(f, "%d\n", storyMaxUnlocked);
    fprintf(f, "%d\n", cheatUnlockAll ? 1 : 0);
    fclose(f);
}

void Game::loadSettings() {
    FILE* f = fopen("settings.cfg", "r");
    if (!f) return;
    char buf[64];
    auto readInt = [&](int& out) -> bool {
        if (!fgets(buf, sizeof(buf), f)) return false;
        char* end = nullptr;
        long v = strtol(buf, &end, 10);
        if (!end || end == buf) return false;
        out = static_cast<int>(v);
        return true;
    };
    int v;
    if (readInt(v)) vsAI = (v != 0);
    if (readInt(v)) {
        if (v < 1) v = 1;
        if (v > 4) v = 4;
        storyMaxUnlocked = v;
    }
    if (readInt(v)) cheatUnlockAll = (v != 0);
    fclose(f);
}

void Game::onSetCompleted(StoryMode::SetId completedSet) {
    int newMin = static_cast<int>(completedSet) + 2;  // Set1→2, ..., FinalBoss→5
    if (newMin > 4) newMin = 4;
    if (newMin > storyMaxUnlocked) {
        storyMaxUnlocked = newMin;
        saveSettings();
    }
}
