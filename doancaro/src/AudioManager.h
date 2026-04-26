#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <string>
#include <vector>
#include "raylib.h"

class AudioManager {
public:
    enum class MusicSlot { None, Menu, Game };

    AudioManager();
    ~AudioManager();

    void init();
    void shutdown();

    void playPlaceSound();
    void playWinSound();
    void playLoseSound();
    void playMenuClickSound();

    // Cuts win + lose mid-playback. Used on Undo / Restart so the long
    // game-over track doesn't keep playing into the next game.
    void stopGameOverSounds();

    // BGM. Each slot has a pool of tracks under assets/sounds/{menu,in-game}/.
    // switchTo*Music() is a no-op when already on that slot, so it's safe to
    // call every frame from the state dispatch. updateMusic() must be called
    // every frame: it pumps the stream and rerolls a new random track when
    // the current one ends.
    void switchToMenuMusic();
    void switchToGameMusic();
    void stopMusic();
    void updateMusic();
    void toggleMusic();
    bool isMusicEnabled() const { return musicEnabled; }

private:
    static constexpr int kPlaceVariants = 5;

    bool initialized;
    bool musicEnabled;
    MusicSlot currentSlot;
    Music currentMusic;
    std::vector<std::string> menuTrackPaths;
    std::vector<std::string> gameTrackPaths;

    Sound placeSounds[kPlaceVariants];
    Sound winSound;
    Sound loseSound;
    Sound menuClickSound;

    void switchSlot(MusicSlot slot);
    void loadRandomFromCurrentSlot();
};

#endif
