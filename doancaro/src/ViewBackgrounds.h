#pragma once

enum class ViewBackgroundId {
    Settings,
    Multiplayer,
    SaveLoad,
};

namespace ViewBackgrounds {

void preload();
void draw(ViewBackgroundId id, int screenW, int screenH);
void shutdown();

}  // namespace ViewBackgrounds
