#ifndef SAVELOADSCREEN_H
#define SAVELOADSCREEN_H

#include "FileManager.h"

class AudioManager;

enum class SlotScreenMode { Save, Load };
enum class SlotScreenResult { None, Selected, Cancelled };

class SaveLoadScreen {
public:
    SaveLoadScreen();

    void open(SlotScreenMode mode);
    void update(AudioManager& audio);
    void draw();

    SlotScreenResult getResult() const { return result; }
    int getSelectedSlot() const { return selectedSlot; }
    SlotScreenMode getMode() const { return mode; }

private:
    SlotScreenMode mode;
    SlotScreenResult result;
    int selectedSlot;
    int deleteConfirmSlot;  // -1 = no pending delete

    // Cached headers
    SaveHeader headers[MAX_SLOTS];
    bool slotOccupied[MAX_SLOTS];

    void refreshSlotInfo();
    bool canSelect(int slot) const;
    void drawSlotCard(int slot, int x, int y, int width, int height, bool selected);
};

#endif
