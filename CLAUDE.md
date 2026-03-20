# intro-oop1 — OOP1 (C++ Programming)

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
- **Research:** Course analysis and strategy docs in `research/`

## Directory Structure
```
intro-oop1/
├── week1/          # Weekly homework
├── week2/          #   Each week: exercise.md + 25310023.cpp
├── ...
├── baigiang/       # Lecture slides (PPSX)
├── extracted_content/  # Markdown from slides
├── research/       # Course analysis, style guide, algo reference
├── .serena/        # Serena MCP config (project-scoped)
└── .mcp.json       # MCP servers config
```

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
| Replace a function/method body | `mcp__serena-oop1__replace_symbol_body` | `Edit` |
| Add code after a symbol | `mcp__serena-oop1__insert_after_symbol` | `Edit` |
| Add code before a symbol | `mcp__serena-oop1__insert_before_symbol` | `Edit` |
| Rename a symbol project-wide | `mcp__serena-oop1__rename_symbol` or `mcp__jetbrains__rename_refactoring` | `Edit` with replace_all |
| Small text replacement in file | `mcp__jetbrains__replace_text_in_file` | `Edit` |
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

**Editing code (symbolic, precise):**
1. `find_symbol` to locate the symbol
2. `replace_symbol_body` for whole-symbol replacement
3. `insert_before_symbol` / `insert_after_symbol` for adding new code
4. `rename_symbol` for renaming across codebase

**Project knowledge:** `write_memory`, `read_memory`, `list_memories`

### JetBrains (`mcp__jetbrains__*`) — IDE Operations

Always pass `projectPath="/home/larvartar/nhannht-projects/hcmus/semester2/intro-oop1"`.

| Task | Tool |
|---|---|
| Get file errors/warnings | `get_file_problems` |
| Project diagnostics (toolchain, CMake) | `get_diagnostic_info` |
| Build the project | `build_project` |
| Run a configuration | `execute_run_configuration` |
| Open file in editor | `open_file_in_editor` |
| Auto-format code | `reformat_file` |
| Safe rename refactoring | `rename_refactoring` |
| Small text edit in file | `replace_text_in_file` |
| Create new file | `create_new_file` |
| Search by text/regex | `search_in_files_by_text`, `search_in_files_by_regex` |
| Find files by name/glob | `find_files_by_name_keyword`, `find_files_by_glob` |
| View compiler/SDK info | `get_compiler_info` |

### Decision flowchart

1. **Need to understand a code file?** → `get_symbols_overview` first, then `find_symbol` with `include_body=true`
2. **Need to edit a function/class?** → `find_symbol` to read, then `replace_symbol_body` to rewrite
3. **Need to add new code?** → `insert_after_symbol` or `insert_before_symbol`
4. **Need to rename?** → `rename_symbol` (Serena) or `rename_refactoring` (JetBrains)
5. **Need to find usages?** → `find_referencing_symbols`
6. **Need to search?** → `search_for_pattern` (Serena) or `search_in_files_by_text` (JetBrains)
7. **Need to check errors?** → `get_file_problems` (per file) or `build_project` (whole project)
8. **Non-code file?** → Use built-in Read/Edit/Write tools

## raylib Gotchas
- **GLB material mapping**: `LoadModel("file.glb")` puts blank default at `materials[0]`, loaded texture at `materials[1]`. Fix: `model.meshMaterial[0] = 1;`
- **GLB model stripping**: Remove node mesh refs with Python, then `npx @gltf-transform/cli prune in.glb out.glb` to shrink file
- **Working directory**: Must run `./CaroGame` from `doancaro/build/` — assets use relative paths. Running from another dir causes silent asset load failures.
- **GLFW init error**: Kill old game process before relaunching: `pkill -f CaroGame`
- **New .cpp files**: Must add to `SOURCES` list in `doancaro/CMakeLists.txt`

## Caro Project Assets
- 3D models in `doancaro/assets/models/` (copied to build dir by CMake post-build)
- Raw/original models in `doancaro/.raw-assets/` (gitignored, not bundled)
- Board model: `table.glb` — Go board from Sketchfab, wood texture, 35 units wide, scaled to fit 12x12 grid

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
