# Caro Game — OOP1 Project

**Group:**
- Nguyen Huu Thien Nhan — 25310023
- Bui Thi Minh Hang — 25310057
- Pham Ngoc Tram — 25310043

![Demo](demo.png)

## What is in this bundle

Everything required to build the game from scratch, fully offline. No downloads, no Internet, no extra packages to install.

```
CaroGame_src/
├── CMakeLists.txt      ← build script
├── README.md
├── demo.png
├── src/                ← game source (C++14)
├── assets/             ← models, pieces, textures, fonts
└── raylib-5.5/         ← raylib library source (bundled)
```

## Build on Windows 11 + Visual Studio 2022

Requirements: Visual Studio 2022 with **"Desktop development with C++"** workload (includes CMake).

### Option A — Open folder in Visual Studio 2022 (easiest)

1. Extract the ZIP.
2. Open Visual Studio 2022 → **Open** → **Folder** → select this extracted folder.
3. Wait for CMake configuration to finish (shown in the Output panel).
4. Top toolbar: set build target to `CaroGame.exe`, set configuration to `x64-Release`.
5. **Build** → **Build All** (Ctrl+Shift+B).
6. **Debug** → **Start Without Debugging** (Ctrl+F5).

### Option B — Command line (Developer Command Prompt for VS 2022)

```cmd
cd path\to\CaroGame_src
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
cd Release
CaroGame.exe
```

### Option C — Any platform with CMake + C++14 compiler

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
./CaroGame
```

## Running the game

After build, the executable is in `build/Release/` (Visual Studio) or `build/` (Makefile / Ninja). The `assets/` folder is copied automatically next to the binary — **run the executable from its own directory** so it finds its assets.

## Controls

- **Mouse**: left-click to place pieces, right-drag to rotate camera, scroll wheel to zoom
- **Keyboard**: WASD / Arrow keys to move cursor, Enter to place
- **ESC**: return to menu (in-game) / exit (from menu)
- **Ctrl+S / Ctrl+L**: quick save / quick load
- **F3**: toggle AI debug panel

## Features

- 15×15 board with 3D Go-stone pieces (unique wood grain per piece)
- Orbital camera (rotate, zoom, reset)
- Three AI difficulty levels — all minimax + α-β pruning, same evaluation function:
  - **Easy** — depth 1 greedy
  - **Normal** — depth 2
  - **Hard** — depth 3 + transposition table
- Cinematic piece-drop animations with squash/stretch and shadows
- Save / Load (3 manual slots + autosave, CRC32-checksummed binary format)
- Undo support
- Settings persist between sessions (`settings.cfg` next to the executable)

## Troubleshooting

- **Black window / missing pieces**: the `assets/` folder must sit next to `CaroGame.exe`. The build copies it automatically; if you move the exe, copy `assets/` with it.
- **CMake error "Visual Studio generator not found"**: install the "Desktop development with C++" workload in Visual Studio Installer.
- **Link error about OpenGL / GDI**: this should not happen — raylib's CMake handles those. If it does, make sure you built with the x64 platform.
