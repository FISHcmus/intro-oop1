#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

class AudioManager;

struct GameSettings {
    bool vsAI;
    bool cheatUnlockAll;   // Story Mode picker shows all 4 sets when true.
};

class SettingsScreen {
public:
    SettingsScreen();

    void update(AudioManager& audio);
    void draw();

    bool isDone() const { return done; }
    void reset();

    GameSettings getSettings() const { return settings; }
    void setSettings(const GameSettings& s) { settings = s; }

private:
    GameSettings settings;
    bool done;

    static const int ITEM_COUNT = 3;  // Mode (info), Cheat toggle, Back

    const char* getGameModeLabel() const;
};

#endif
