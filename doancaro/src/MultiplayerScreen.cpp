#include "MultiplayerScreen.h"

#include "AudioManager.h"
#include "Fonts.h"
#include "Theme.h"
#include "UIComponents.h"
#include "ViewBackgrounds.h"
#include "raylib.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>

namespace {

constexpr float kPanelW = 620.0f;
constexpr float kBtnW = 340.0f;
constexpr float kBtnH = 52.0f;
constexpr float kFieldH = 62.0f;
constexpr float kGap = 14.0f;

Rectangle centeredRect(float y, float w, float h) {
    return { (GetScreenWidth() - w) * 0.5f, y, w, h };
}

UIC::State buttonState(Rectangle r, bool enabled = true) {
    if (!enabled) return UIC::State::Disabled;
    Vector2 mouse = GetMousePosition();
    if (!CheckCollisionPointRec(mouse, r)) return UIC::State::Rest;
    return IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? UIC::State::Pressed
                                                : UIC::State::Focused;
}

void drawInputField(Rectangle r, const char* label, const std::string& value,
                    bool active, bool roomCode) {
    Color border = active ? Theme::palette.gold_foil
                          : Theme::withAlpha(Theme::palette.son_bone, 160);
    DrawRectangleRounded(r, 0.18f, 6,
                         Theme::withAlpha(Theme::palette.ink_sumi, 180));
    DrawRectangleRoundedLinesEx(r, 0.18f, 6, 2.0f, border);

    Fonts::draw(Fonts::bold, label, static_cast<int>(r.x) + 16,
                static_cast<int>(r.y) + 10, 15.0f, Theme::palette.gold_foil);

    std::string shown = value.empty()
        ? (roomCode ? "Enter room code" : "Type here")
        : value;
    Color textColor = value.empty()
        ? Theme::withAlpha(Theme::palette.son_bone, 120)
        : Theme::palette.paper_washi;
    Fonts::draw(Fonts::body, shown.c_str(), static_cast<int>(r.x) + 16,
                static_cast<int>(r.y) + 32, 18.0f, textColor);

    if (active) {
        int x = static_cast<int>(r.x) + 18
              + Fonts::measure(Fonts::body, shown.c_str(), 18.0f);
        DrawRectangle(x, static_cast<int>(r.y) + 32, 2, 20,
                      Theme::palette.gold_foil);
    }
}

void drawSelectorField(Rectangle r, const char* label, const char* value,
                       bool focused) {
    Color border = focused ? Theme::palette.gold_foil
                           : Theme::withAlpha(Theme::palette.son_bone, 160);
    DrawRectangleRounded(r, 0.18f, 6,
                         Theme::withAlpha(Theme::palette.ink_sumi, 180));
    DrawRectangleRoundedLinesEx(r, 0.18f, 6, 2.0f, border);

    Fonts::draw(Fonts::bold, label, static_cast<int>(r.x) + 16,
                static_cast<int>(r.y) + 10, 15.0f, Theme::palette.gold_foil);
    Fonts::draw(Fonts::body, value, static_cast<int>(r.x) + 16,
                static_cast<int>(r.y) + 32, 18.0f, Theme::palette.paper_washi);
    Fonts::draw(Fonts::body, "Click to switch", static_cast<int>(r.x) + 440,
                static_cast<int>(r.y) + 32, 15.0f,
                Theme::withAlpha(Theme::palette.son_bone, 180));
}

ServerConfig::OnlineEndpointPreset nextPreset(
    ServerConfig::OnlineEndpointPreset preset) {
    return (preset == ServerConfig::OnlineEndpointPreset::Production)
        ? ServerConfig::OnlineEndpointPreset::Local
        : ServerConfig::OnlineEndpointPreset::Production;
}

}  // namespace

MultiplayerScreen::MultiplayerScreen() {
    reset();
}

void MultiplayerScreen::reset() {
    view = View::Root;
    activeField = 0;
    playerName = "Player";
    address = "127.0.0.1";
    portText = std::to_string(ServerConfig::defaultLanPort());
    roomCode.clear();
    showAdvanced = false;
    endpointPreset = ServerConfig::defaultPreset();
    heading = "MULTIPLAYER";
    statusLine = "LAN uses direct IP. Online uses a room server.";
    detailLine.clear();
    pendingAction = {};
}

MultiplayerAction MultiplayerScreen::consumeAction() {
    MultiplayerAction out = pendingAction;
    pendingAction = {};
    return out;
}

void MultiplayerScreen::setWaitingView(const std::string& newHeading,
                                       const std::string& status,
                                       const std::string& detail) {
    view = View::Waiting;
    activeField = -1;
    heading = newHeading;
    statusLine = status;
    detailLine = detail;
}

void MultiplayerScreen::setStatusMessage(const std::string& status,
                                         const std::string& detail) {
    statusLine = status;
    if (!detail.empty()) {
        detailLine = detail;
        return;
    }

    if (view == View::Waiting && !roomCode.empty()) {
        detailLine = "Room code: " + roomCode;
        return;
    }

    detailLine.clear();
}

void MultiplayerScreen::setRoomCode(const std::string& code) {
    roomCode = code;
    detailLine = "Room code: " + code;
}

void MultiplayerScreen::update(AudioManager& audio) {
    switch (view) {
        case View::Root:       updateRoot(audio); break;
        case View::LanHost:
        case View::LanJoin:
        case View::OnlineHost:
        case View::OnlineJoin: updateForm(audio); break;
        case View::Waiting:    updateWaiting(audio); break;
    }
}

void MultiplayerScreen::draw() {
    ViewBackgrounds::draw(ViewBackgroundId::Multiplayer,
                          GetScreenWidth(), GetScreenHeight());

    UIC::drawTitle(heading.c_str(), GetScreenWidth(), GetScreenHeight());
    switch (view) {
        case View::Root:       drawRoot(); break;
        case View::LanHost:
        case View::LanJoin:
        case View::OnlineHost:
        case View::OnlineJoin: drawForm(); break;
        case View::Waiting:    drawWaiting(); break;
    }
}

void MultiplayerScreen::updateRoot(AudioManager& audio) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        audio.playMenuClickSound();
        queueAction(MultiplayerActionType::BackToMenu);
        return;
    }

    struct Choice { const char* label; View next; };
    static const Choice choices[] = {
        {"HOST LAN GAME", View::LanHost},
        {"JOIN LAN BY IP", View::LanJoin},
        {"CREATE ONLINE ROOM", View::OnlineHost},
        {"JOIN ONLINE ROOM", View::OnlineJoin},
        {"BACK", View::Root},
    };

    float y = GetScreenHeight() * 0.28f;
    for (int i = 0; i < 5; ++i) {
        Rectangle r = centeredRect(y, kBtnW, kBtnH);
        if (CheckCollisionPointRec(GetMousePosition(), r) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            audio.playMenuClickSound();
            if (i == 4) {
                queueAction(MultiplayerActionType::BackToMenu);
            } else {
                view = choices[i].next;
                heading = choices[i].label;
                statusLine.clear();
                detailLine.clear();
                activeField = 0;
                showAdvanced = false;
            }
        }
        y += kBtnH + kGap;
    }
}

void MultiplayerScreen::updateForm(AudioManager& audio) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        audio.playMenuClickSound();
        reset();
        return;
    }

    struct FieldSpec {
        const char* label;
        FieldKind kind;
        std::string* value;
        int maxLen;
    };

    FieldSpec fields[4];
    int fieldCount = 0;
    fields[fieldCount++] = {"PLAYER NAME", FieldKind::Name, &playerName, 24};
    if (view == View::LanJoin) {
        fields[fieldCount++] = {"HOST IP", FieldKind::Address, &address, 48};
    }
    if (view == View::LanHost || view == View::LanJoin) {
        fields[fieldCount++] = {"PORT", FieldKind::Port, &portText, 5};
    }
    if (view == View::OnlineJoin) {
        fields[fieldCount++] = {"ROOM CODE", FieldKind::RoomCode, &roomCode, 8};
    }

    float startY = GetScreenHeight() * 0.24f;
    for (int i = 0; i < fieldCount; ++i) {
        Rectangle r = centeredRect(startY + i * (kFieldH + 10.0f), kPanelW, kFieldH);
        if (CheckCollisionPointRec(GetMousePosition(), r) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            activeField = i;
        }
    }

    if (activeField >= 0 && activeField < fieldCount) {
        int codepoint = GetCharPressed();
        while (codepoint > 0) {
            if (static_cast<int>(fields[activeField].value->size()) < fields[activeField].maxLen &&
                isFieldCharAllowed(fields[activeField].kind, codepoint)) {
                if (fields[activeField].kind == FieldKind::RoomCode) {
                    fields[activeField].value->push_back(
                        static_cast<char>(std::toupper(codepoint)));
                } else {
                    fields[activeField].value->push_back(static_cast<char>(codepoint));
                }
            }
            codepoint = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !fields[activeField].value->empty()) {
            fields[activeField].value->pop_back();
        }
    }

    float controlsY = startY + fieldCount * (kFieldH + 10.0f);
    if (isOnlineView() && ServerConfig::advancedSelectorEnabled()) {
        Rectangle advancedRect = centeredRect(controlsY + 6.0f, kBtnW, 42.0f);
        if (CheckCollisionPointRec(GetMousePosition(), advancedRect) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            audio.playMenuClickSound();
            showAdvanced = !showAdvanced;
        }
        controlsY += 52.0f;

        if (showAdvanced) {
            Rectangle presetRect = centeredRect(controlsY, kPanelW, kFieldH);
            if (CheckCollisionPointRec(GetMousePosition(), presetRect) &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                audio.playMenuClickSound();
                endpointPreset = nextPreset(endpointPreset);
            }
            controlsY += kFieldH + 10.0f;
        }
    }

    MultiplayerActionType submit = MultiplayerActionType::None;
    switch (view) {
        case View::LanHost:    submit = MultiplayerActionType::StartLanHost; break;
        case View::LanJoin:    submit = MultiplayerActionType::StartLanJoin; break;
        case View::OnlineHost: submit = MultiplayerActionType::StartOnlineHost; break;
        case View::OnlineJoin: submit = MultiplayerActionType::StartOnlineJoin; break;
        default: break;
    }

    float buttonY = controlsY + 24.0f;
    Rectangle submitRect = centeredRect(buttonY, kBtnW, kBtnH);
    Rectangle backRect = centeredRect(buttonY + kBtnH + 10.0f, kBtnW, kBtnH);

    if (CheckCollisionPointRec(GetMousePosition(), submitRect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        audio.playMenuClickSound();
        queueAction(submit);
        return;
    }
    if (CheckCollisionPointRec(GetMousePosition(), backRect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        audio.playMenuClickSound();
        reset();
        return;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        audio.playMenuClickSound();
        queueAction(submit);
    }
}

void MultiplayerScreen::updateWaiting(AudioManager& audio) {
    Rectangle cancelRect = centeredRect(GetScreenHeight() * 0.24f + 230.0f - 72.0f,
                                        kBtnW, kBtnH);
    if (CheckCollisionPointRec(GetMousePosition(), cancelRect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        audio.playMenuClickSound();
        queueAction(MultiplayerActionType::CancelWaiting);
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        audio.playMenuClickSound();
        queueAction(MultiplayerActionType::CancelWaiting);
    }
}

void MultiplayerScreen::drawRoot() {
    Fonts::drawCentered(Fonts::body,
                        "LAN uses host IP. Online uses room codes.",
                        GetScreenWidth() / 2, GetScreenHeight() / 6 + 52,
                        18.0f, Theme::withAlpha(Theme::palette.son_bone, 210));

    static const char* labels[] = {
        "HOST LAN GAME",
        "JOIN LAN BY IP",
        "CREATE ONLINE ROOM",
        "JOIN ONLINE ROOM",
        "BACK",
    };

    float y = GetScreenHeight() * 0.28f;
    for (int i = 0; i < 5; ++i) {
        Rectangle r = centeredRect(y, kBtnW, kBtnH);
        UIC::drawPrimaryButton(r, labels[i], buttonState(r));
        y += kBtnH + kGap;
    }

    UIC::drawHintBar("Mouse to pick a path · ESC returns to menu",
                     GetScreenWidth(), GetScreenHeight());
}

void MultiplayerScreen::drawForm() {
    const char* note = "";
    switch (view) {
        case View::LanHost:
            note = "Host a game on your local network. Share your local IP after starting.";
            break;
        case View::LanJoin:
            note = "Join the host directly with local IP and port.";
            break;
        case View::OnlineHost:
            note = "Create a room. Other players only need the room code.";
            break;
        case View::OnlineJoin:
            note = "Join an online room with a room code. The server stays hidden.";
            break;
        default:
            break;
    }

    Fonts::drawCentered(Fonts::body, note, GetScreenWidth() / 2,
                        GetScreenHeight() / 6 + 52, 16.0f,
                        Theme::withAlpha(Theme::palette.son_bone, 210));

    struct DrawField {
        const char* label;
        const std::string* value;
        bool roomCode;
    };

    DrawField fields[4];
    int fieldCount = 0;
    fields[fieldCount++] = {"PLAYER NAME", &playerName, false};
    if (view == View::LanJoin) {
        fields[fieldCount++] = {"HOST IP", &address, false};
    }
    if (view == View::LanHost || view == View::LanJoin) {
        fields[fieldCount++] = {"PORT", &portText, false};
    }
    if (view == View::OnlineJoin) {
        fields[fieldCount++] = {"ROOM CODE", &roomCode, true};
    }

    float startY = GetScreenHeight() * 0.24f;
    for (int i = 0; i < fieldCount; ++i) {
        Rectangle r = centeredRect(startY + i * (kFieldH + 10.0f), kPanelW, kFieldH);
        drawInputField(r, fields[i].label, *fields[i].value,
                       activeField == i, fields[i].roomCode);
    }

    float controlsY = startY + fieldCount * (kFieldH + 10.0f);
    if (isOnlineView() && ServerConfig::advancedSelectorEnabled()) {
        const char* toggle = showAdvanced ? "ADVANCED: ON" : "ADVANCED: OFF";
        Rectangle advancedRect = centeredRect(controlsY + 6.0f, kBtnW, 42.0f);
        UIC::drawPrimaryButton(advancedRect, toggle, buttonState(advancedRect));
        controlsY += 52.0f;

        if (showAdvanced) {
            Rectangle presetRect = centeredRect(controlsY, kPanelW, kFieldH);
            drawSelectorField(presetRect, "SERVER TARGET",
                              ServerConfig::presetLabel(endpointPreset),
                              CheckCollisionPointRec(GetMousePosition(), presetRect));
            Fonts::drawCentered(Fonts::mono,
                                ServerConfig::presetSummary(endpointPreset).c_str(),
                                GetScreenWidth() / 2,
                                static_cast<int>(controlsY + kFieldH + 6.0f),
                                13.0f,
                                Theme::withAlpha(Theme::palette.son_bone, 190));
            controlsY += kFieldH + 24.0f;
        }
    }

    const char* submit = "START";
    if (view == View::OnlineHost) submit = "CREATE ROOM";
    else if (view == View::OnlineJoin) submit = "JOIN ROOM";
    else if (view == View::LanHost) submit = "HOST";
    else if (view == View::LanJoin) submit = "CONNECT";

    float buttonY = controlsY + 24.0f;
    Rectangle submitRect = centeredRect(buttonY, kBtnW, kBtnH);
    Rectangle backRect = centeredRect(buttonY + kBtnH + 10.0f, kBtnW, kBtnH);
    UIC::drawPrimaryButton(submitRect, submit, buttonState(submitRect));
    UIC::drawPrimaryButton(backRect, "BACK", buttonState(backRect));

    if (!statusLine.empty()) {
        Fonts::drawCentered(Fonts::body, statusLine.c_str(), GetScreenWidth() / 2,
                            static_cast<int>(buttonY) + 132, 15.0f,
                            Theme::palette.gold_foil);
    }
    if (!detailLine.empty()) {
        Fonts::drawCentered(Fonts::mono, detailLine.c_str(), GetScreenWidth() / 2,
                            static_cast<int>(buttonY) + 160, 14.0f,
                            Theme::withAlpha(Theme::palette.son_bone, 220));
    }

    const char* hint = isOnlineView()
        ? "Enter submits · Players use room codes · Advanced is for dev builds"
        : "Click a field to edit · Enter submits · ESC goes back";
    UIC::drawHintBar(hint, GetScreenWidth(), GetScreenHeight());
}

void MultiplayerScreen::drawWaiting() {
    Rectangle panel = centeredRect(GetScreenHeight() * 0.24f, kPanelW, 230.0f);
    DrawRectangleRounded(panel, 0.18f, 8,
                         Theme::withAlpha(Theme::palette.ink_sumi, 180));
    DrawRectangleRoundedLinesEx(panel, 0.18f, 8, 2.0f,
                                Theme::palette.gold_foil);

    Fonts::drawCentered(Fonts::bold, heading.c_str(), GetScreenWidth() / 2,
                        static_cast<int>(panel.y) + 28, 28.0f,
                        Theme::palette.paper_washi);
    Fonts::drawCentered(Fonts::body, statusLine.c_str(), GetScreenWidth() / 2,
                        static_cast<int>(panel.y) + 86, 18.0f,
                        Theme::palette.gold_foil);
    if (!detailLine.empty()) {
        Fonts::drawCentered(Fonts::mono, detailLine.c_str(),
                            GetScreenWidth() / 2,
                            static_cast<int>(panel.y) + 126, 18.0f,
                            Theme::palette.paper_washi);
    }

    Rectangle cancelRect = centeredRect(panel.y + panel.height - 72.0f, kBtnW, kBtnH);
    UIC::drawPrimaryButton(cancelRect, "CANCEL", buttonState(cancelRect));

    UIC::drawHintBar("Waiting for peer / server · Enter or ESC cancels",
                     GetScreenWidth(), GetScreenHeight());
}

bool MultiplayerScreen::isOnlineView() const {
    return view == View::OnlineHost || view == View::OnlineJoin;
}

void MultiplayerScreen::queueAction(MultiplayerActionType type) {
    pendingAction = {};
    pendingAction.type = type;
    pendingAction.playerName = playerName.empty() ? "Player" : playerName;
    pendingAction.address = address.empty() ? "127.0.0.1" : address;
    pendingAction.roomCode = roomCode;
    pendingAction.port = parsedPort();
    pendingAction.endpointPreset = endpointPreset;
}

int MultiplayerScreen::parsedPort() const {
    int port = ServerConfig::defaultLanPort();
    if (!portText.empty()) {
        port = std::max(1, std::min(65535, std::atoi(portText.c_str())));
    }
    return port;
}

bool MultiplayerScreen::isFieldCharAllowed(FieldKind kind, int codepoint) {
    if (codepoint < 32 || codepoint > 126) return false;
    unsigned char ch = static_cast<unsigned char>(codepoint);
    switch (kind) {
        case FieldKind::Name:
            return std::isalnum(ch) || ch == ' ' || ch == '_' || ch == '-';
        case FieldKind::Address:
            return std::isalnum(ch) || ch == '.' || ch == ':' || ch == '-';
        case FieldKind::Port:
            return std::isdigit(ch) != 0;
        case FieldKind::RoomCode:
            return std::isalnum(ch) != 0;
    }
    return false;
}
