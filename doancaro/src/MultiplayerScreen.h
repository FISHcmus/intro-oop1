#pragma once

#include "ServerConfig.h"

#include <string>

class AudioManager;

enum class MultiplayerActionType {
    None,
    BackToMenu,
    StartLanHost,
    StartLanJoin,
    StartOnlineHost,
    StartOnlineJoin,
    CancelWaiting,
};

struct MultiplayerAction {
    MultiplayerActionType type = MultiplayerActionType::None;
    std::string playerName;
    std::string address;
    std::string roomCode;
    int port = 0;
    ServerConfig::OnlineEndpointPreset endpointPreset =
        ServerConfig::OnlineEndpointPreset::Production;
};

class MultiplayerScreen {
public:
    MultiplayerScreen();

    void reset();
    void update(AudioManager& audio);
    void draw();

    MultiplayerAction consumeAction();

    void setWaitingView(const std::string& title,
                        const std::string& status,
                        const std::string& detail);
    void setStatusMessage(const std::string& status,
                          const std::string& detail = std::string());
    void setRoomCode(const std::string& roomCode);

private:
    enum class View {
        Root,
        LanHost,
        LanJoin,
        OnlineHost,
        OnlineJoin,
        Waiting,
    };

    enum class FieldKind {
        Name,
        Address,
        Port,
        RoomCode,
    };

    View view;
    int activeField;

    std::string playerName;
    std::string address;
    std::string portText;
    std::string roomCode;
    bool showAdvanced;
    ServerConfig::OnlineEndpointPreset endpointPreset;

    std::string heading;
    std::string statusLine;
    std::string detailLine;

    MultiplayerAction pendingAction;

    void updateRoot(AudioManager& audio);
    void updateForm(AudioManager& audio);
    void updateWaiting(AudioManager& audio);

    void drawRoot();
    void drawForm();
    void drawWaiting();

    bool isOnlineView() const;
    void queueAction(MultiplayerActionType type);
    int parsedPort() const;

    static bool isFieldCharAllowed(FieldKind kind, int codepoint);
};
