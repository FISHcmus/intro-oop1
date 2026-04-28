# intro-oop1 — OOP1 (C++ Programming)

## Web Interaction

**Always use Chrome DevTools MCP** (`mcp__chrome-devtools__*`) for ALL web page interaction — fetching content, navigating, submitting forms, reading pages. NEVER use WebFetch or WebSearch unless the user explicitly asks for it. The browser is already open with an active CDP session.

## Course Info
- **Student ID:** 25310023
- **Language:** C++14 strict (must load into Visual Studio before submission)
- **Homework naming:** `25310023_25310057_25310043.cpp` (group submission, MSSV1_MSSV2_MSSV3)
- **Group members:**
  - Nguyen Huu Thien Nhan — 25310023 (always first)
  - Bui Thi Minh Hang — 25310057
  - Pham Ngoc Tram — 25310043
- **Final project:** Caro game with raylib (see `DoAnCaro.pdf`)
- **Slides:** PPSX in `baigiang/`, extracted markdown in `extracted_content/`
- **Class pacing:** Painfully slow — by week 9 (2026-04-29) the class has barely cleared week 5 content (File Processing). Slides go up to week 10 but in-class progress lags ~3 weeks behind.
- **Research:** Course analysis and strategy docs in `research/`

## Directory Structure

```
intro-oop1/
├── doancaro/                    # Final project — Caro game (raylib, C++14)
│   ├── src/                     #   18 modules — see "Caro Architecture" below
│   ├── tools/gallery_main.cpp   #   CaroGallery dev binary (UI sandbox)
│   ├── tests/                   #   Catch2 v3 unit tests (test_board, test_ai)
│   ├── assets/                  #   fonts/, images/, models/, sounds/, pieces/, textures/, videos/
│   ├── slides/                  #   Slidev defense deck (caro-slides, package.json)
│   ├── report/                  #   our_work/ (.md, .org, .tex, .pdf) + example/
│   ├── storyline/               #   storyline_final.org — Cô Sử Tiên 4-set Story Mode arc
│   ├── public/                  #   screenshots used in the report
│   ├── AI_ALGORITHM.md          #   AI design doc
│   ├── CMakeLists.txt           #   3 targets (CaroGame, CaroGallery, CaroTests)
│   └── .raw-assets/             #   gitignored — original GLB sources before stripping
│
├── week1/ … week3/, week7/, week8/   # Weekly homework dirs (note: 4, 5, 6 skipped — not yet done)
├── homework_lythuyet/lythuyetlan5/   # Theory homework
├── baigiang/                    # Lecture slides — PPSX (Week01–Week10)
├── extracted_content/           # Markdown extracted from PPSX (CSLTr_Week*.md + week*_images/)
├── research/                    # 5 strategy/reference docs (algorithms, style, libraries, course)
├── books/                       # Reference PDFs — Stroustrup C++, CLRS Algorithms
├── past_exams/                  # Mid-term archives (2023-2024 + 2026 thuchanh)
├── ci-assets/                   # Visual Studio submission bundle (.sln, .vcxproj, bundle CMakeLists)
├── cf/                          # Codeforces problem sets (gym W1-W2, W3-W4, W5-W6, OOP-Challenge)
├── saves/                       # Runtime save data (.cdat slots)
├── cmake-build-debug/           # CLion debug build dir
├── cmake-build-release/         # CLion release build dir
├── DoAnCaro.pdf                 # Final-project specification (teacher's rubric)
├── DeCuongChiTietHocPhan_*.pdf  # Official syllabus
├── midterm_cheatsheet.md, note*.md   # Hand-written prep notes
├── compile_commands.json        # clangd / Serena LSP index
├── .serena/                     # Serena MCP config (project-scoped)
└── .mcp.json                    # MCP servers config
```

## Caro Game Architecture (doancaro/)

### Three build targets

| Target | Source | Role |
|---|---|---|
| `CaroGame` | src/* (~14 .cpp incl. main.cpp) | Production binary |
| `CaroGallery` | tools/gallery_main.cpp + CaroUI lib | Dev sandbox — exercises every UIC component in every state, no game state. Window 1100×700, ESC to quit |
| `CaroTests` | tests/test_board.cpp, tests/test_ai.cpp + GameLogic lib | Catch2 v3.8.0 unit tests, FetchContent + CTest discovery |

### Two internal static libraries

| Library | Sources | Linked by |
|---|---|---|
| `CaroUI` | src/UIComponents.cpp, src/Theme.cpp, src/Fonts.cpp | CaroGame, CaroGallery |
| `GameLogic` | src/Board.cpp, src/Player.cpp, src/AIPlayer.cpp | CaroTests |

### Source modules — one-line role

| File | Role |
|---|---|
| `main.cpp` | 6-line entry: `Game game; game.run();` |
| `Game.{h,cpp}` | Top-level state machine (`enum class GameState { Menu, Settings, PickDifficulty, Playing, GameOver, SaveScreen, LoadScreen }`); owns Board + 2 Players + Renderer + every screen + AudioManager. Async AI thread. Move history for undo. Toast + debug panel. |
| `Board.{h,cpp}` | 15×15 grid. `placeMove/undoMove`, win detection, `getCandidateMoves` for AI, Zobrist hash for TT. |
| `Player.{h,cpp}` | Base class. Human `setNextMove` from input. Stats. |
| `AIPlayer.{h,cpp}` | Minimax + alpha-beta + transposition table (`unordered_map<uint64_t, TTEntry>`) + 243-entry pattern table (3⁵ window encoding). Threat-space search code present but unwired. `lastDebug` exposes top-5 moves, depth completed, TT stats. |
| `Renderer.{h,cpp}` | 3D scene — orbital camera, 6 GLB models, **5 shaders**: gloss/Phong wet-coat, sky 3-stop gradient, edge-fade scroll, vignette, multi-octave value-noise mist. Async texture loader (450 unique piece grains). Camera-breathing idle bob. |
| `MenuScreen.{h,cpp}` | Items: NewGame / **StoryMode** / LoadGame / Settings / Exit. preload() / shutdown() for ~200 MB animated background frames (121 PNGs). |
| `SettingsScreen.{h,cpp}` | vsAI toggle + Back. |
| `DifficultyScreen.{h,cpp}` | Easy / Normal / Hard / Cancel. Maps to AI search depth 1/2/3. |
| `SaveLoadScreen.{h,cpp}` | 4 slots (0=autosave, 1–3 manual). Save/Load mode. Per-slot delete confirm. Slot card preview. |
| `GameScreen.{h,cpp}` | HUD draw, message draw, prompt with text input. |
| `AudioManager.{h,cpp}` | Place/Win/Lose/MenuClick SFX. BGM with menu/in-game pools, random reroll on track end. |
| `FileManager.{h,cpp}` | Save format. Magic="CARO" (0x4341524F), version 3, CRC32 checksum, 4 slots. Header has stats + AI depth + game mode. |
| `ParticleSystem.{h,cpp}` | 3D particles. Shapes: Cube/FlatRect/Spark. Emitters: placement / win-celebration / landing. Pending burst queue. |
| `Theme.{h,cpp}` | Wuxia-storm palette (son_jade, son_bone, thuy_cyan, thuy_pearl, ink_sumi, gold_foil, etc.). Tokens: spacing, radius, elevation, motion. Type slots: `display_brush`, `body_serif`, `mono` (mono is declared but unset). |
| `Fonts.{h,cpp}` | Inter Regular / Inter Bold / Bebas Neue. **Loaded with `LoadFontEx(..., nullptr, 0)` → ASCII 32–126 only.** Vietnamese diacritics + UTF-8 box-drawing chars currently render as `?`. |
| `UIComponents.{h,cpp}` | UIC namespace: `drawPrimaryButton`, `drawTitle`, `drawHintBar`. State enum (Rest/Focused/Pressed/Disabled). Stateless free functions. |
| `UI.h` | One inline helper: `mouseMoved()` — true only on frames the mouse delta is non-zero. |

### Build optimizations (CMakeLists.txt)

- **ccache** auto-detected → `CMAKE_{C,CXX}_COMPILER_LAUNCHER`
- **mold linker** auto-detected → `-fuse-ld=mold` for native builds
- `-pipe` always; `-gsplit-dwarf` in Debug
- **PCH** for CaroGame: `<vector>`, `<string>`, `<cmath>`, `<cstdio>` (raylib.h excluded — quoted-include resolution clashes with PCH)
- **PCH** for CaroTests: `<catch2/catch_test_macros.hpp>`, `<vector>`, `<string>`
- raylib 5.5 via FetchContent (URL fallback when system pkg absent)
- POST_BUILD copies `assets/` next to BOTH `CaroGame` and `CaroGallery` binaries

### Where new code goes

| Adding… | Goes in… |
|---|---|
| New game-logic .cpp (board/AI/player) | `SOURCES` (CaroGame) AND `GAME_LOGIC_SOURCES` (CaroTests) in CMakeLists |
| New screen / Game-only feature | `SOURCES` (CaroGame) only |
| New UI primitive (button, panel, text widget) | `CaroUI` library — auto-available to CaroGame + CaroGallery |
| New Catch2 test file | `add_executable(CaroTests …)` line in CMakeLists |
| New gallery showcase | `tools/gallery_main.cpp` directly (one binary) |

## Code Editing Tools (MANDATORY)

**CRITICAL: For ALL code files, you MUST use Serena MCP and JetBrains MCP tools instead of built-in Read/Edit/Grep/Write. Built-in tools are ONLY for non-code files (markdown, PDFs, JSON config, etc.).**

The projectPath for all JetBrains calls is `/home/larvartar/nhannht-projects/hcmus/semester2/intro-oop1`.

Serena uses LSP backend (clangd). JetBrains backend not supported on CLion — see https://github.com/oraios/serena/issues/1193.

### When to use which tool

| Action | Tool | NOT this |
|---|---|---|
| See what's in a code file | `mcp__serena-oop1__get_symbols_overview` | `Read` |
| Read a specific function/class body | `mcp__serena-oop1__find_symbol` with `include_body=true` | `Read` |
| Read a full code file (rare, avoid) | `mcp__jetbrains__get_file_text_by_path` | `Read` |
| Search code for a pattern | `mcp__serena-oop1__search_for_pattern` or `mcp__jetbrains__search_in_files_by_text` | `Grep` |
| Find files by name | `mcp__jetbrains__find_files_by_name_keyword` or `mcp__serena-oop1__find_file` | `Glob` |
| Replace text in a code file | `mcp__jetbrains__replace_text_in_file` | `Edit` |
| Add code after a symbol | `mcp__serena-oop1__insert_after_symbol` | `Edit` |
| Add code before a symbol | `mcp__serena-oop1__insert_before_symbol` | `Edit` |
| Rename a symbol project-wide | `mcp__serena-oop1__rename_symbol` or `mcp__jetbrains__rename_refactoring` | `Edit` with replace_all |
| Check for errors/warnings | `mcp__jetbrains__get_file_problems` | `Bash` g++ |
| Find who references a symbol | `mcp__serena-oop1__find_referencing_symbols` | `Grep` |
| Create a new code file | `mcp__jetbrains__create_new_file` | `Write` |
| Format a file | `mcp__jetbrains__reformat_file` | nothing |

### Serena (`mcp__serena-oop1__*`) — Semantic Code Analysis

**Exploring code (token-efficient, start here):**
1. `get_symbols_overview` — get file structure without reading full source
2. `find_symbol` with `include_body=true` — read only the symbol you need
3. `find_referencing_symbols` — find all usages of a symbol
4. `search_for_pattern` — regex search across codebase
5. Fall back to full file read only when necessary

**Editing code (use JetBrains for all text replacement):**
1. `find_symbol` to locate the symbol and understand the code
2. `mcp__jetbrains__replace_text_in_file` for ALL code edits (replacements, rewrites, fixes)
3. `insert_before_symbol` / `insert_after_symbol` ONLY for inserting new code blocks (not replacing)
4. `rename_symbol` for renaming across codebase
5. **DO NOT use `replace_symbol_body`** — it is unreliable. Always use JetBrains `replace_text_in_file` instead.

**Project knowledge:** `write_memory`, `read_memory`, `list_memories`

### JetBrains (`mcp__jetbrains__*`) — IDE Operations

Always pass `projectPath="/home/larvartar/nhannht-projects/hcmus/semester2/intro-oop1"`.

| Task | Tool |
|---|---|
| Get file errors/warnings | `get_file_problems` |
| Project diagnostics (toolchain, CMake) | `get_diagnostic_info` |
| Run a configuration (launches exe in IDE) | `execute_run_configuration` |
| Open file in editor | `open_file_in_editor` |
| Auto-format code | `reformat_file` |
| Safe rename refactoring | `rename_refactoring` |
| Edit/replace code in file (primary editing tool) | `replace_text_in_file` |
| Create new file | `create_new_file` |
| Search by text/regex | `search_in_files_by_text`, `search_in_files_by_regex` |
| Find files by name/glob | `find_files_by_name_keyword`, `find_files_by_glob` |
| View compiler/SDK info | `get_compiler_info` |

### Build Rule (MANDATORY)

**NEVER use `mcp__jetbrains__build_project` to build.** It returns `{"isSuccess": true/false, "problems": [{"message": "The project has limited build diagnostics functionality. Build messages cannot be collected."}]}` — pass/fail only, no compiler messages, no file/line info. Useless for debugging.

**ALWAYS build via CLI:**
```bash
cmake --build /home/larvartar/nhannht-projects/hcmus/semester2/intro-oop1/cmake-build-debug --target <target> 2>&1 | tail -50
```
Gives full stderr with file:line errors, warnings, linker output — everything needed to debug.

**IDE MCP is for DIAGNOSIS, not building:**
- `get_file_problems` — per-file errors/warnings from CLion inspection (no build needed)
- `get_diagnostic_info` — toolchain / CMake setup health check
- `get_compiler_info` — compiler version and flags

Exception: `execute_run_configuration` is fine for launching a built exe (Run ▶). It's not a build tool.

### Decision flowchart

1. **Need to understand a code file?** → `get_symbols_overview` first, then `find_symbol` with `include_body=true`
2. **Need to edit a function/class?** → `find_symbol` to read, then `mcp__jetbrains__replace_text_in_file` to rewrite
3. **Need to add new code?** → `insert_after_symbol` or `insert_before_symbol`
4. **Need to rename?** → `rename_symbol` (Serena) or `rename_refactoring` (JetBrains)
5. **Need to find usages?** → `find_referencing_symbols`
6. **Need to search?** → `search_for_pattern` (Serena) or `search_in_files_by_text` (JetBrains)
7. **Need to check per-file errors?** → `get_file_problems` (no build, fast)
8. **Need to build?** → CLI: `cmake --build cmake-build-debug --target <target>` (see Build Rule above — NEVER use `build_project` MCP)
9. **Non-code file?** → Use built-in Read/Edit/Write tools

## raylib Gotchas
- **GLB material mapping (single-material only)**: For GLBs with `materialCount == 1`, raylib prepends a blank default; bump with `model.meshMaterial[i] = 1;` for every mesh. For multi-material GLBs (`materialCount >= 2`) raylib already maps correctly — applying the bump shifts mappings off-by-one and breaks rendering. Always check `model.materialCount` before deciding.
- **PBR GLB renders dark**: Models authored for PBR engines often use `baseColorFactor=(0,0,0)` + `emissive` (e.g. `fluffy_cloud.glb`). raylib's default shader ignores emissive, so the model multiplies down to near-black. Fix: `for (int i=0; i<model.materialCount; ++i) model.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;` after `LoadModel`.
- **Cross-plane / transparent GLBs draw a "cube outline"**: Models built as 3 perpendicular plane cards (cloud-style billboards, fluffy_cloud.glb) show a hard bbox-edge outline because each card's transparent fragments still write depth, occluding the other cards behind them — the visible clip line is the bbox boundary. **Why:** raylib's default pipeline has depth-WRITE enabled even for alpha-blended fragments. **Fix:** wrap the draw with `rlDisableDepthMask(); /* DrawModelEx ... */ rlEnableDepthMask();` — turns off depth writes (test still runs, so clouds stay correctly hidden behind the board) so transparent fragments stop blocking siblings. Same rule applies to particle quads and any multi-card billboard.
- **Cloud instance `c.scale` is world-units, not jitter**: After `baseScale = 10.0f / maxSpan` and `c.scale = baseScale * jitter`, `c.scale` ranges ~3.0–35.0 world units, NOT the input jitter ~0.3–3.5. Any extent math (fade radius, bbox checks) must use `c.scale * 0.5` for half-width — not `c.scale * 5`.
- **Renderer default camera distance = 35**: Sanity-check any world-space radius against this. A fade-out radius >35 means every object sits inside its own invisibility ring at default zoom.
- **GLB model stripping**: Remove node mesh refs with Python, then `npx @gltf-transform/cli prune in.glb out.glb` to shrink file
- **Working directory**: Must run `./CaroGame` from `doancaro/build/` — assets use relative paths. Running from another dir causes silent asset load failures.
- **GLFW init error**: Kill old game process before relaunching: `pkill -f CaroGame`
- **New .cpp files**: See "Where new code goes" table above — destination depends on target (CaroGame `SOURCES`, `CaroUI` lib, `GameLogic` lib, or `CaroTests`).

## Caro Project Assets

CMake POST_BUILD copies the entire `assets/` tree next to BOTH `CaroGame` and `CaroGallery` binaries. Raw/original sources live in `doancaro/.raw-assets/` (gitignored, not bundled).

### `assets/fonts/` — 3 TTFs

| File | Role | Notes |
|---|---|---|
| `Inter-Regular.ttf` | body / HUD | Currently loaded ASCII-only |
| `Inter-Bold.ttf` | emphasis / player names | Currently loaded ASCII-only |
| `BebasNeue.ttf` | titles / big text | Currently loaded ASCII-only |

**No monospace font yet** — `Theme::type.mono` slot is declared in Theme.h but never assigned. Required for any UTF-8 box-drawing layout (panels with `╔═╗`).

### `assets/models/` — 7 GLBs

| File | Role | Notes |
|---|---|---|
| `table.glb` | 15×15 Go-style board | 35 units wide, baked grid |
| `fire_rune.glb` | PlayerX (Sơn Tinh) piece | Talisman GLB overrides sphere fallback |
| `water_rune.glb` | PlayerO (Thủy Tinh) piece | Same |
| `floating_island.glb` | Pedestal under board | Auto-centered via bbox fit |
| `mountain_and_river_scroll.glb` | Backdrop behind board | KHR_materials_unlit anime-style; do NOT apply glossShader |
| `fluffy_cloud.glb` | Drifting cloud instances | 3-plane billboard — needs `rlDisableDepthMask` wrapper (see raylib gotchas) |
| `birds.glb` | (unused — kept in tree for now) | — |

### `assets/pieces/` — 450 pre-rendered piece textures

`light_<row>_<col>.png` and `dark_<row>_<col>.png`, one PNG per cell × 2 colors = 15 × 15 × 2. Async loaded by `Renderer::texLoaderThread`, uploaded to GPU one-per-frame via `uploadPendingTextures()`.

### `assets/sounds/` — SFX + 9 BGM tracks

| Path | Files |
|---|---|
| `assets/sounds/place_0[0-4].ogg` | 5 piece-placement SFX (rotated to dodge repetition) |
| `assets/sounds/win.ogg`, `lose.ogg`, `menu_click.ogg` | One-shot SFX |
| `assets/sounds/menu/` | 3 menu-BGM tracks: Forest_Ambience, ObservingTheStar, TownTheme |
| `assets/sounds/in-game/` | 6 game-BGM tracks: 003_Vaporware, Ambient Piano, dova_Eternal Terror, gone_fishin (CC0), sadpianov1, where-was-i |

`AudioManager::updateMusic()` rerolls a random track when the current one ends.

### `assets/images/` — menu chrome

| Path | Role |
|---|---|
| `menu-background-1.png` | Static menu background |
| `menu-button.png` | Menu button base sprite |
| `buttons/` | exit/play-via-internet/play-with-ai/settings PNGs |
| `menu-frames/` | **121-frame animated menu background** (~200 MB VRAM — `MenuScreen::preload/shutdown` manages it) |

### `assets/videos/`, `assets/textures/`

| File | Role |
|---|---|
| `videos/menu-background.mp4` | Menu video backdrop |
| `videos/setting-background.mp4` | Settings screen video backdrop |
| `textures/ground.png` | Generic ground texture |

### Runtime config

- `settings.cfg` in build dir — persists `vsAI` and `aiDepth` between restarts
- Save slots: `slot0.cdat` (autosave) … `slot3.cdat` in user dir, NOT bundle dir

## Story Mode (storyline_final.org)

`MenuScreen::MenuChoice` already includes `StoryMode` — wired into the enum, no impl yet. The narrative spec is in `doancaro/storyline/storyline_final.org` ("Cô Sử Tiên 4-set arc"). Three sets at fixed difficulties (Easy/Medium/Hard) plus a final boss set, each with intro / win / lose narration beats and per-set linh-vật unlock lines (Voi 9 ngà / Gà 9 cựa / Ngựa 9 hồng mao).

## Homework Structure (MANDATORY — applies to ALL homework except Caro project)

Every homework file (lý thuyết + thực hành) MUST follow this exact layout. This rule OVERRIDES the C++14 preference in "Code Style Rules" for homework files.

**Guiding principle: use C syntax as much as possible.** Teacher's own lecture code mixes C and C++ (e.g. `int& n` references inside otherwise-C code), but our submissions stay stricter — prefer C idioms in every choice where both work. When forced to pick (e.g. file extension must be `.cpp`), pick the smallest C++ concession.

1. **Use C language, not C++** — `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<math.h>`, `<ctype.h>`. No `iostream`, no `std::`, no `cout`/`cin`, no references (output params use `T*` pointer, not `T&`), no classes, no templates, no STL, no `new`/`delete` (use `malloc`/`free`).
2. **File extension stays `.cpp`** — submission naming `MSSV1_MSSV2_..._MSSV5.cpp` is enforced by Moodle, so the file must compile as C++ even though the code is pure C. Write code that is valid as BOTH C and C++ (verify with `gcc -x c -std=c11 -Wall -Wextra` AND `g++ -std=c++14 -Wall -Wextra`).
3. **Fixed code layout** in this order:
   ```
   // 1. #include directives
   // 2. Struct / typedef declarations (if any)
   // 3. Khoi khai bao ham     — function prototypes
   // 4. Ham main              — int main() { ... }
   // 5. Khoi dinh nghia ham   — function bodies
   ```
   Prototypes block sits ABOVE `main`; definitions block sits BELOW `main`. Never inline function bodies before `main`.
4. **Caro project is exempt** — it uses C++14 with raylib and keeps full C++ features.

## Code Style Rules

- **Clear, simple C++14** — readable code, split into small functions, handle all edge cases
- **Use all semester techniques freely** — don't limit to what's been taught that week
- **Only course-taught libraries** — `<cstdio>`, `<cstdlib>`, `<cmath>`, `<iostream>`, `<string>`, `<vector>`, `<cstring>`, `<cctype>`, `<ctime>`, `<fstream>`. No external/advanced libraries in homework.
- **Caro project exempt** — can use raylib and additional libraries as needed
- **Linter/IDE > teacher's outdated patterns** — when clang-tidy or IDE flags something, follow the modern standard:
  - `int main()` not `void main()`
  - `<cstdio>` not `<stdio.h>`
  - `strtol`/`strtoll` not `scanf` for integer conversion
  - `static_cast<>()` not C-style casts
- Vietnamese function/variable names: `tinhTuoi`, `docSoNguyen`, `menhGia`, `soNgayTrongThang`

## Quality Checklist (after every code change)

1. `g++ -std=c++14` — must compile clean
2. `mcp__jetbrains__get_file_problems` with `errorsOnly: false` — zero warnings
3. `lizard file.cpp` — all functions CCN < 15 (installed via `uv tool install lizard`)
4. Test all edge cases via piped input: `echo "input" | ./program`

## Code Quality

After writing or editing any code, ALWAYS run `mcp__jetbrains__get_file_problems` to check for errors and warnings. Fix all issues before considering the task done.

**Inline diagnostics:** When JetBrains diagnostics appear in `<new-diagnostics>` tags during edits, NEVER ignore them. Fix every diagnostic immediately — even info-level hints. No exceptions.

## Verification (MANDATORY)

**Every code change MUST be compiled and tested before considering it done.** No exceptions — always run at least one of these methods after any edit:

### 1. Automated Programmatic Test
- Write a test that pipes input via `echo` and checks output with `grep`/`diff`
- Example: `echo "15 3 2000" | ./program | grep "Your age: 26"`
- Good for regression testing and quick validation of expected output

### 2. Manual E2E Test via tmux
- Use tmux to run the compiled program in a separate pane for interactive testing
- Workflow:
  1. `tmux split-window -h` — open a side pane
  2. Compile: `g++ -std=c++14 -o program file.cpp`
  3. Run interactively in the split pane: `tmux send-keys -t 1 './program' Enter`
  4. Send input: `tmux send-keys -t 1 '15 3 2000' Enter`
  5. Read output: `tmux capture-pane -t 1 -p`
- Good for interactive programs, verifying UI/UX flow, and programs that require multi-step input

### 3. Code Coverage (gcovr)
- Compile with `--coverage`: `g++ -std=c++14 --coverage -o /tmp/prog file.cpp`
- Run program with test inputs, then: `gcovr --root . --filter <dir>/ --object-directory /tmp`
- For HTML report: `gcovr --root . --filter <dir>/ --object-directory /tmp --html-details coverage.html`
- Aim for high coverage on multi-function code (Caro project, larger exercises)
- Tools installed: `gcov` (GCC), `lcov` (HTML), `gcovr` (terminal + HTML/XML)
