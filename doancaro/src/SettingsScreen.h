#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

struct GameSettings {
    bool vsAI;
    int aiDepth;  // 1=Easy (greedy one-ply, no minimax), 2=Normal (d=2), 3=Hard (d=3)
};

class SettingsScreen {
public:
    SettingsScreen();

    void update();
    void draw();

    bool isDone() const { return done; }
    void reset();

    GameSettings getSettings() const { return settings; }
    void setSettings(const GameSettings& s) { settings = s; }

private:
    GameSettings settings;
    int selectedIndex;
    bool done;

    static const int ITEM_COUNT = 3;  // Game Mode, AI Difficulty, Back

    const char* getDifficultyLabel() const;
    const char* getGameModeLabel() const;
};

#endif
