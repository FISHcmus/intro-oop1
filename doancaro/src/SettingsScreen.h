#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

class AudioManager;

struct GameSettings {
    bool vsAI;
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
    int selectedIndex;
    bool done;

    static const int ITEM_COUNT = 2;  // Game Mode, Back

    const char* getGameModeLabel() const;
};

#endif
