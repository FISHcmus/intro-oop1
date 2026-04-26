#ifndef MENUSCREEN_H
#define MENUSCREEN_H

class AudioManager;

enum class MenuChoice { None, NewGame, LoadGame, Settings, Exit };

class MenuScreen {
public:
    MenuScreen();

    void update(AudioManager& audio);
    void draw();

    MenuChoice getChoice() const { return choice; }
    void reset();

private:
    int selectedIndex;
    MenuChoice choice;
    static const int ITEM_COUNT = 4;
    const char* items[ITEM_COUNT];
};

#endif
