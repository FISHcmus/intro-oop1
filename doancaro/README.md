# Caro Game — OOP1 Project Demo

**Group:** Nguyen Huu Thien Nhan (25310023), Bui Thi Minh Hang (25310057), Pham Ngoc Tram (25310043)

## Quick Start (Windows)

1. Extract the ZIP
2. Run `CaroGame.exe`

## Controls

- **Mouse**: Click to place pieces, right-drag to rotate camera, scroll to zoom
- **Keyboard**: WASD/Arrow keys to move cursor, Enter to place
- **ESC**: Return to menu (from game) / Exit (from menu)
- **Ctrl+S / Ctrl+L**: Quick save/load

## Build from Source

Requirements: CMake 3.16+, C++14 compiler (GCC, MSVC, Clang). No other dependencies — raylib is downloaded automatically.

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Visual Studio (Windows)

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 15 2017" -A x64
```
Then open `CaroGame.sln` and build (Release, x64).

## Features

- 2.5D board with orbital camera (rotate, zoom, reset)
- AI opponent with 3 difficulty levels (Easy/Medium/Hard)
- Threat-space search for forced win detection
- Iterative deepening with time limit (1s medium, 3s hard)
- Save/Load game (3 slots)
- Settings changeable mid-game
