# Caro Project Strategy

## Mandatory Features (from DoAnCaro.pdf)

The teacher's template provides ~80% of the architecture. These 5 features are explicitly listed as "missing" and must be implemented:

### 4.1 Save/Load Game
- Press **'L'** → prompt for filename → save current state
- Press **'T'** → prompt for filename → load saved state
- Implementation: `fprintf`/`fscanf` the board array `_A[][]`, `_TURN`, `_X`, `_Y`

### 4.2 Win/Lose/Draw Detection (`TestBoard()`)
- Check 4 directions: horizontal, vertical, diagonal-left, diagonal-right
- Count consecutive same-color marks from last placed position
- 5 in a row = win
- All cells filled + no winner = draw
- This is the **hardest feature** — edge cases at board boundaries

### 4.3 Win/Lose/Draw Visual Effects
- Current code just prints a text line
- Need: highlight winning line, animation, color changes
- Can use `SetConsoleTextAttribute()` for colors on Windows

### 4.4 In-Game UI
- Display player stats: moves count, wins per player
- Show whose turn it is
- Clear layout with labels

### 4.5 Main Menu
- "New Game", "Load Game", "Settings", "Exit"
- Navigation with arrow keys or numbered options
- Settings could include: board size, player names

## Architecture (from teacher's template)

**MVC Pattern:**
- **Model:** `ResetData()`, `CheckBoard()`, `TestBoard()` — data and logic
- **View:** `DrawBoard()`, `ProcessFinish()`, `GotoXY()` — display
- **Control:** `main()` loop, `MoveUp/Down/Left/Right()` — input handling

**Global State:**
```cpp
#define BOARD_SIZE 12
#define LEFT 3
#define TOP 1

struct _POINT { int x, y, c; };  // x: screen col, y: screen row, c: mark (0/-1/1)
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;        // true = player 1 (X), false = player 2 (O)
int _COMMAND;      // key pressed
int _X, _Y;        // current cursor position
```

**Convention:** `c = 0` (empty), `c = -1` (player 1 / X), `c = 1` (player 2 / O)

## Console vs Raylib

| | Console (teacher's template) | Raylib |
|---|---|---|
| **Grading risk** | Low — matches exactly what teacher expects | Higher — teacher may not have raylib |
| **Complexity** | Moderate — Windows API (`conio.h`, `windows.h`) | Moderate — different API |
| **Visual quality** | Basic (text characters) | Much better (graphics) |
| **Portability** | Windows only | Cross-platform |

**Recommendation:** Build console version first (guaranteed grading compatibility), raylib as optional bonus.

## Implementation Priority

1. **TestBoard()** — win detection (5 in a row, 4 directions) + draw detection
2. **Save/Load** — file I/O with board state serialization
3. **Main menu** — simple text menu with switch-case
4. **Player stats UI** — display alongside board
5. **Visual effects** — console colors for winning line

## Win Detection Algorithm (TestBoard)

For each direction (horizontal, vertical, 2 diagonals):
1. From last placed position, count consecutive same-color in both directions
2. If count >= 5, that player wins

```
Directions to check from position (row, col):
- Horizontal: (0, +1) and (0, -1)
- Vertical: (+1, 0) and (-1, 0)
- Diagonal \: (+1, +1) and (-1, -1)
- Diagonal /: (+1, -1) and (-1, +1)
```
