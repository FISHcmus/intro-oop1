#pragma once

// Compile-time storyline text — Vietnamese with diacritics, rendered through
// Fonts::mono (Vietnamese Extended range loaded at init).
// Source: doancaro/storyline/storyline_final.org

namespace StoryContent {

// Intro monologue — paginated by paragraph block. Each page is one
// ComicPanel body. Page count is fixed; index 0..kIntroPageCount-1.
constexpr int kIntroPageCount = 6;
extern const char* const kIntroPages[kIntroPageCount];

// Set narration. Each set has one intro line, one win line, one lose line.
// Final boss adds a special boss-reveal panel with ASCII art "plot" block.
struct SetText {
    const char* title;     // panel title bar, e.g. "MƯA GIÔNG"
    const char* tag;       // [TAG] right side of title bar, e.g. "SET 1"
    const char* intro;     // narration before first match
    const char* win;       // narration after winning the set
    const char* lose;      // narration after losing the set
};

extern const SetText kSet1;
extern const SetText kSet2;
extern const SetText kSet3;
extern const SetText kFinalBoss;

// ASCII art for the boss reveal — drawn in the ComicPanel.plot field
// for FinalBoss.intro. ~24 lines, ≤ 70 cols.
extern const char* const kThuyTinhBossArt;

// Linh vật unlock lore lines — shown after winning a set.
extern const char* const kVoiUnlockLine;
extern const char* const kGaUnlockLine;
extern const char* const kNguaUnlockLine;

// Closing line after final boss win → Menu.
extern const char* const kEpilogueLine;

}  // namespace StoryContent
