#include "AudioManager.h"
#include <cstdlib>

namespace {
    // SFX paths are relative to the working dir (the build dir, where assets/
    // is copied by the CMake post-build step). Swap a file by replacing it on
    // disk — no rebuild needed.
    const char* kPlacePaths[] = {
        "assets/sounds/place_00.ogg",
        "assets/sounds/place_01.ogg",
        "assets/sounds/place_02.ogg",
        "assets/sounds/place_03.ogg",
        "assets/sounds/place_04.ogg",
    };
    const char* kWinPath       = "assets/sounds/win.ogg";
    const char* kLosePath      = "assets/sounds/lose.ogg";
    const char* kMenuClickPath = "assets/sounds/menu_click.ogg";

    const char* kMenuMusicDir  = "assets/sounds/menu";
    const char* kGameMusicDir  = "assets/sounds/in-game";

    constexpr float kPlaceVolume     = 0.6f;
    constexpr float kWinVolume       = 0.8f;
    constexpr float kLoseVolume      = 0.7f;
    constexpr float kMenuClickVolume = 0.5f;
    constexpr float kMusicVolume     = 0.3f;  // BGM sits under SFX

    Sound loadIfExists(const char* path) {
        Sound s{};
        if (FileExists(path)) s = LoadSound(path);
        return s;
    }

    void unloadIfLoaded(Sound& s) {
        if (IsSoundValid(s)) {
            UnloadSound(s);
            s = Sound{};
        }
    }

    void playIfLoaded(Sound& s, float volume) {
        if (!IsSoundValid(s)) return;
        SetSoundVolume(s, volume);
        PlaySound(s);
    }

    void stopIfPlaying(Sound& s) {
        if (IsSoundValid(s) && IsSoundPlaying(s)) StopSound(s);
    }

    void scanOggDir(const char* dir, std::vector<std::string>& out) {
        if (!DirectoryExists(dir)) return;
        FilePathList files = LoadDirectoryFiles(dir);
        for (unsigned i = 0; i < files.count; i++) {
            const char* p = files.paths[i];
            const char* ext = GetFileExtension(p);
            if (ext && TextIsEqual(ext, ".ogg")) out.emplace_back(p);
        }
        UnloadDirectoryFiles(files);
    }
}

AudioManager::AudioManager()
    : initialized(false), musicEnabled(true),
      currentSlot(MusicSlot::None), currentMusic{},
      placeSounds{}, winSound{}, loseSound{}, menuClickSound{} {}

AudioManager::~AudioManager() {
    if (initialized) shutdown();
}

void AudioManager::init() {
    if (initialized) return;
    InitAudioDevice();
    initialized = true;

    for (int i = 0; i < kPlaceVariants; i++) {
        placeSounds[i] = loadIfExists(kPlacePaths[i]);
    }
    winSound       = loadIfExists(kWinPath);
    loseSound      = loadIfExists(kLosePath);
    menuClickSound = loadIfExists(kMenuClickPath);

    scanOggDir(kMenuMusicDir, menuTrackPaths);
    scanOggDir(kGameMusicDir, gameTrackPaths);
}

void AudioManager::shutdown() {
    if (!initialized) return;
    stopMusic();
    for (auto& s : placeSounds) unloadIfLoaded(s);
    unloadIfLoaded(winSound);
    unloadIfLoaded(loseSound);
    unloadIfLoaded(menuClickSound);
    CloseAudioDevice();
    initialized = false;
}

void AudioManager::playPlaceSound() {
    int idx = std::rand() % kPlaceVariants;
    playIfLoaded(placeSounds[idx], kPlaceVolume);
}

void AudioManager::playWinSound()      { playIfLoaded(winSound,       kWinVolume); }
void AudioManager::playLoseSound()     { playIfLoaded(loseSound,      kLoseVolume); }
void AudioManager::playMenuClickSound(){ playIfLoaded(menuClickSound, kMenuClickVolume); }

void AudioManager::stopGameOverSounds() {
    stopIfPlaying(winSound);
    stopIfPlaying(loseSound);
}

void AudioManager::switchToMenuMusic() { switchSlot(MusicSlot::Menu); }
void AudioManager::switchToGameMusic() { switchSlot(MusicSlot::Game); }

void AudioManager::switchSlot(MusicSlot slot) {
    if (!initialized || !musicEnabled) return;
    // Gate on slot identity only — if the slot's pool is empty or load
    // fails, we stay silent for this slot rather than retrying every frame.
    if (currentSlot == slot) return;
    stopMusic();
    currentSlot = slot;
    loadRandomFromCurrentSlot();
}

void AudioManager::loadRandomFromCurrentSlot() {
    const auto& pool = (currentSlot == MusicSlot::Menu) ? menuTrackPaths : gameTrackPaths;
    if (pool.empty()) return;
    int idx = std::rand() % static_cast<int>(pool.size());
    currentMusic = LoadMusicStream(pool[idx].c_str());
    if (!IsMusicValid(currentMusic)) return;
    SetMusicVolume(currentMusic, kMusicVolume);
    PlayMusicStream(currentMusic);
}

void AudioManager::stopMusic() {
    if (!IsMusicValid(currentMusic)) return;
    StopMusicStream(currentMusic);
    UnloadMusicStream(currentMusic);
    currentMusic = Music{};
}

void AudioManager::updateMusic() {
    if (!IsMusicValid(currentMusic)) return;
    UpdateMusicStream(currentMusic);
    if (!IsMusicStreamPlaying(currentMusic)) {
        UnloadMusicStream(currentMusic);
        currentMusic = Music{};
        loadRandomFromCurrentSlot();
    }
}

void AudioManager::toggleMusic() {
    musicEnabled = !musicEnabled;
    if (!musicEnabled) {
        stopMusic();
        currentSlot = MusicSlot::None;  // force reroll on re-enable
    }
}
