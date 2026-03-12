# intro-oop1 — OOP1 (C++ Programming)

## Course Info
- **Student ID:** 25310023
- **Language:** C++14 strict (must load into Visual Studio before submission)
- **Homework naming:** `MSSV_______.cpp` (e.g. `25310023.cpp`)
- **Final project:** Caro game with raylib (see `DoAnCaro.pdf`)
- **Slides:** PPSX in `baigiang/`, extracted markdown in `extracted_content/`

## Directory Structure
```
intro-oop1/
├── week1/          # Weekly homework
├── week2/          #   Each week: exercise.md + 25310023.cpp
├── ...
├── baigiang/       # Lecture slides (PPSX)
├── extracted_content/  # Markdown from slides
├── .serena/        # Serena MCP config (project-scoped)
└── .mcp.json       # MCP servers config
```

## MCP Tools Workflow

This project has two complementary MCP tool sets for code intelligence. Use them together.

### Serena (`mcp__serena-oop1__*`) — Semantic Code Analysis

Serena provides **symbol-aware** code operations. Prefer Serena for:

| Task | Tool |
|---|---|
| Explore project files | `list_dir`, `find_file` |
| Search code patterns (regex) | `search_for_pattern` |
| Get symbols in a file (classes, functions) | `jet_brains_get_symbols_overview` |
| Find a specific symbol by name | `jet_brains_find_symbol` |
| Find who references a symbol | `jet_brains_find_referencing_symbols` |
| Get class hierarchy (super/sub) | `jet_brains_type_hierarchy` |
| Replace a function/class body | `replace_symbol_body` |
| Insert code before/after a symbol | `insert_before_symbol`, `insert_after_symbol` |
| Rename across codebase | `rename_symbol` |
| Store project knowledge | `write_memory`, `read_memory`, `list_memories` |

**Important:** Serena's `jet_brains_*` tools require CLion open with Serena plugin on this project. File-based tools (`list_dir`, `find_file`, `search_for_pattern`, memories) always work.

### JetBrains (`mcp__jetbrains__*`) — IDE Operations

JetBrains MCP provides **IDE-level** operations. Prefer JetBrains for:

| Task | Tool |
|---|---|
| Get file errors/warnings | `get_file_problems` |
| Project diagnostics (toolchain, CMake) | `get_diagnostic_info` |
| Build the project | `build_project` |
| Run a configuration | `execute_run_configuration` |
| Run terminal commands in IDE | `execute_terminal_command` |
| Open file in editor | `open_file_in_editor` |
| Auto-format code | `reformat_file` |
| Safe rename refactoring | `rename_refactoring` |
| Search by text/regex across project | `search_in_files_by_text`, `search_in_files_by_regex` |
| Find files by name/glob | `find_files_by_name_keyword`, `find_files_by_glob` |
| View compiler/SDK info | `get_compiler_info` |
| View run configurations | `get_run_configurations` |
| View project dependencies | `get_project_dependencies` |

### Combined Workflow

**Reading code (token-efficient):**
1. `serena-oop1::jet_brains_get_symbols_overview` — get file structure without reading full source
2. `serena-oop1::jet_brains_find_symbol` with `include_body=true` — read only the symbol you need
3. Fall back to full file read only when necessary

**Editing code:**
1. Use `serena-oop1::jet_brains_find_symbol` to locate the symbol
2. Use `serena-oop1::replace_symbol_body` for whole-symbol replacement
3. Use `serena-oop1::insert_before_symbol` / `insert_after_symbol` for adding new code
4. Use Claude Code's `Edit` tool for small line-level edits within a symbol

**Diagnosing issues:**
1. `jetbrains::get_file_problems` — get errors/warnings for a specific file
2. `jetbrains::get_diagnostic_info` — check toolchain/CMake configuration
3. `jetbrains::build_project` — verify it compiles

**Refactoring:**
1. `serena-oop1::jet_brains_find_referencing_symbols` — find all usages before changing
2. `serena-oop1::rename_symbol` or `jetbrains::rename_refactoring` — safe rename
3. `jetbrains::get_file_problems` — verify no breakage after refactor

**Building and running:**
1. `jetbrains::build_project` — compile
2. `jetbrains::get_run_configurations` — list available configs
3. `jetbrains::execute_run_configuration` — run

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
