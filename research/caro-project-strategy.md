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

## Decision: Full Raylib 2.5D (skip console version)

The PDF says "Caro game with raylib" is the final project. Going all-in on raylib with a creative 2.5D approach.

**Platform:** Linux, software rendering (llvmpipe, OpenGL 4.5). No GPU — keep geometry simple.

## Notable University & Open-Source References

### Academic Projects

| Source | Approach | Link |
|---|---|---|
| Stanford CS224R | Deep learning (ResNet, HybridNet + self-attention) for Gomoku AI | [PDF](https://cs224r.stanford.edu/projects/pdfs/CS_224R_Final_Report-4.pdf) |
| Cal Poly Thesis | AlphaZero-style: residual neural net (policy + value heads), no human knowledge | [Thesis](https://digitalcommons.calpoly.edu/cgi/viewcontent.cgi?article=3178&context=theses) |
| Syracuse CIS667 | CNN + minimax heuristic function | [GitHub](https://github.com/rasi5050/Gomoku-AI) |
| Rapfi (2025) | Distilling efficient neural networks for Gomoku | [arXiv](https://arxiv.org/html/2503.13178v1) |

### Industrial-Grade Open Source

| Project | Architecture | Notable Features |
|---|---|---|
| [Carbon-Gomoku](https://github.com/gomoku/Carbon-Gomoku) | C++, minimax + alpha-beta | Transposition table, pattern lookup tables (256x256), candidate generation, expert knowledge. Heavily documented |
| [kigster/gomoku-ansi-c](https://github.com/kigster/gomoku-ansi-c) | ANSI C99 + React + Rails 8 | **Best architecture reference** — stateless HTTP AI daemon + ReactJS frontend + PostgreSQL. JSON game state. Cluster behind reverse proxy |
| [blupig-gomoku](https://github.com/yunzhu-li/blupig-gomoku) | C++, negamax + alpha-beta | Custom heuristic, clean codebase |
| [AlphaZero_Gomoku](https://github.com/junxiaosong/AlphaZero_Gomoku) | Python, MCTS + neural net | AlphaZero implementation, 3.4k stars |
| [lgarczyn/gomoku](https://github.com/lgarczyn/gomoku) | C++ AI + graphic interface | Has GUI, good visual layer reference |

### Raylib References

| Resource | Description |
|---|---|
| [Isometric Grid (raylib)](https://github.com/daniel-meilak/Isometric-Grid) | Pixel art isometric grid with raylib |
| [Chess in raylib](https://medium.com/@vigneshbezawada3/building-a-chess-game-in-c-with-raylib-part-1-rendering-the-board-pieces-d34ebdc48fad) | Board game rendering tutorial |
| [raylib examples](https://www.raylib.com/examples.html) | 120+ official examples (3D, shaders, audio) |
| [raylib games](https://www.raylib.com/games.html) | Official game collection |

## Creative 2.5D Design Direction

### Art Style Candidates (ranked by llvmpipe feasibility)

1. **Neon/Synthwave** — Dark board, glowing grid lines, neon X/O pieces with bloom effect. Minimal geometry, maximum visual impact. Ideal for software rendering.
2. **Lo-fi / Pixel art 2.5D** — Isometric pixel board, chunky pieces. Trending in 2026 indie games. Very light on GPU.
3. **Wooden/Zen** — Realistic wood texture board, stone pieces (Go-style), soft ambient lighting. Calm aesthetic.
4. **Plushcore / Toy aesthetic** — Soft, toy-like 3D pieces on a fabric board. Trending 2026.

### 2.5D Rendering Approach (raylib)

- `BeginMode3D()` with fixed camera at ~45° angle looking at the board
- Board = textured plane or grid of `DrawCube()` tiles
- Pieces = `DrawCylinder()` or loaded `.obj` models
- Mouse ray-casting (`GetMouseRay()`) for click-to-place (primary input)
- WASD/arrow keys as alternative (teacher's requirement)
- Keep polygon count low for llvmpipe performance

### Visual Effects (llvmpipe-safe)

- Piece placement animation (scale-up bounce)
- Winning line glow/highlight
- Particle burst on win
- Smooth camera transitions between states
- Custom font rendering for menus/HUD

## AI Strategy: Minimax + Alpha-Beta Pruning (DECIDED)

**Decision:** Pure minimax + alpha-beta. No neural networks, no model files, no training.

**Why not AlphaZero?**
- AlphaZero requires a trained neural network model file + inference library (ONNX Runtime ~50MB)
- Training needs GPU or hours of CPU time; pre-trained models exist but add dependency complexity
- Minimax is pure algorithm — zero external dependencies, zero RAM beyond recursion stack
- Explainable to teacher in an OOP1 course; AlphaZero raises questions about ML in an intro course
- A well-tuned minimax at depth 5-6 already plays stronger than most humans on 12x12

**AlphaZero reference (for future/side project):**
- [Augustusmyc/Alpha-Zero-Gomoku](https://github.com/Augustusmyc/Alpha-Zero-Gomoku) — PyTorch train → ONNX → C++ inference
- Pre-trained model: ~10-50 MB RAM, ~10-50ms per move on CPU
- Feasible on this server (Xeon E3-1245, no GPU) for 6x6/8x8 boards; 12x12 training = days

**Implementation details:**
- ~200-300 lines of C++14
- Search depth 4-6 is strong enough for casual play
- Pattern-based evaluation function:
  - Count open-3s, open-4s, blocked-4s for each player
  - Score threats exponentially (open-4 >> open-3 >> open-2)
- Carbon-Gomoku's lookup table approach is the gold standard reference

**Evaluation heuristic (from Carbon-Gomoku):**
- Evaluate each cell separately, sum results (negative for opponent)
- Per-cell: sum of assessments across 4 directions
- Per-direction: count possible winning 5-in-a-rows through that square
- Pre-calculated 256×256 lookup table for efficiency

**Enhancements (implement if time allows):**
- Transposition table (hash seen board states)
- Iterative deepening (search deeper when time allows)
- Candidate move generation (only evaluate moves near existing pieces)

## Proposed Project Architecture

```
doancaro/
├── src/
│   ├── main.cpp              # Entry point, game loop
│   ├── game.h/cpp            # Game state machine, rules, win detection
│   ├── board.h/cpp           # Board data structure, serialization
│   ├── ai.h/cpp              # Minimax + alpha-beta AI opponent
│   ├── renderer.h/cpp        # raylib 2.5D rendering (board, pieces, effects)
│   ├── ui.h/cpp              # Menu, HUD, settings screens
│   ├── audio.h/cpp           # Sound effects, background music
│   └── save_load.h/cpp       # File I/O for game state (JSON or binary)
├── assets/
│   ├── textures/             # Board, piece textures
│   ├── models/               # 3D models (.obj) if used
│   ├── fonts/                # Custom fonts
│   ├── sounds/               # SFX (place piece, win, menu click)
│   └── music/                # Background music (lo-fi, ambient)
├── CMakeLists.txt            # Build system
└── README.md
```

### Design Patterns

- **State machine** for game flow: `Menu → Playing → GameOver → Menu`
- **MVC separation** — game logic knows nothing about rendering
- **RAII wrappers** around raylib resources (`Texture2D`, `Sound`, `Music`)
- **Serialization** — save/load game state as JSON or binary file

## Implementation Priority (revised for raylib)

Tracked in Taskwarrior: `task project:hcmus.oop1.caro list`

### Milestone v1 — Core Game (due 2026-04-07)

| # | Task | Priority | Due | TW ID |
|---|---|---|---|---|
| 1 | CMake + raylib build system | H | 2026-03-24 | 95 |
| 2 | 2.5D board rendering (grid, camera, click-to-place) | H | 2026-03-24 | 96 |
| 3 | Game logic: pieces, turns, win detection | H | 2026-03-28 | 97 |
| 4 | Minimax + alpha-beta AI opponent | H | 2026-04-04 | 98 |
| 5 | Main menu + settings (state machine) | M | 2026-04-07 | 99 |
| 6 | Save/Load game state to file | M | 2026-04-07 | 100 |

### Milestone v2 — Polish (due 2026-04-21)

| # | Task | Priority | Due | TW ID |
|---|---|---|---|---|
| 7 | Visual effects: animations, particles, winning line | M | 2026-04-14 | 101 |
| 8 | Audio: SFX + background music | L | 2026-04-14 | 102 |
| 9 | Polish: custom fonts, transitions, about screen | L | 2026-04-21 | 103 |

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
