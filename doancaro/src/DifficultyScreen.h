#ifndef DIFFICULTYSCREEN_H
#define DIFFICULTYSCREEN_H

class AudioManager;

class DifficultyScreen {
public:
    DifficultyScreen();

    void update(AudioManager& audio);
    void draw();

    bool isDone() const { return done; }
    bool wasCancelled() const { return cancelled; }
    int getChosenDepth() const { return chosenDepth; }
    void reset();

private:
    int selectedIndex;
    bool done;
    bool cancelled;
    int chosenDepth;

    static const int ITEM_COUNT = 4;  // Easy, Normal, Hard, Cancel
};

#endif
