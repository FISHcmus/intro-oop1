#include "AudioManager.h"
#include "raylib.h"

AudioManager::AudioManager() : initialized(false), musicPlaying(false) {}

AudioManager::~AudioManager() {
    if (initialized) {
        shutdown();
    }
}

void AudioManager::init() {
    InitAudioDevice();
    initialized = true;
    // TODO: load sound files from assets/ (task 102)
}

void AudioManager::shutdown() {
    if (initialized) {
        CloseAudioDevice();
        initialized = false;
    }
}

void AudioManager::playPlaceSound() {
    // TODO: load and play (task 102)
}

void AudioManager::playWinSound() {
    // TODO: load and play (task 102)
}

void AudioManager::playMenuClickSound() {
    // TODO: load and play (task 102)
}

void AudioManager::startBackgroundMusic() {
    // TODO: load and play (task 102)
    musicPlaying = true;
}

void AudioManager::stopBackgroundMusic() {
    // TODO: stop (task 102)
    musicPlaying = false;
}

void AudioManager::toggleMusic() {
    if (musicPlaying) stopBackgroundMusic();
    else startBackgroundMusic();
}
