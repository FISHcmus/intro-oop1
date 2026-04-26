#pragma once
#include "raylib.h"

namespace UI {

// True only on frames where the mouse moved. Use to gate hover-driven
// selection so a parked mouse on a list item doesn't snap selectedIndex back
// every frame and silently defeat keyboard navigation.
inline bool mouseMoved() {
    Vector2 d = GetMouseDelta();
    return d.x != 0.0f || d.y != 0.0f;
}

} // namespace UI
