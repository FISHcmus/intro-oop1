#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void init();
    void shutdown();

    void playPlaceSound();
    void playWinSound();
    void playMenuClickSound();

    void startBackgroundMusic();
    void stopBackgroundMusic();
    void toggleMusic();

    bool isMusicPlaying() const { return musicPlaying; }

private:
    bool initialized;
    bool musicPlaying;
};

#endif
