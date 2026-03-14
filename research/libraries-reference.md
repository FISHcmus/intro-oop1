# Libraries Reference for OOP1

## Standard Libraries Used in Course

| Library | Purpose | First appears |
|---|---|---|
| `<stdio.h>` | `printf`, `scanf`, file I/O (`fopen`, `fscanf`, `fprintf`, etc.) | Week 1 |
| `<iostream>` | `cout`, `cin`, `endl` | Week 1 |
| `<math.h>` / `<cmath>` | `sqrt`, `pow`, `floor`, `ceil`, `sin` | Week 1 |
| `<string.h>` / `<cstring>` | `strlen`, `strcpy`, `strcmp`, `strcat`, `strstr`, `strncmp` | Week 4 (strings) |
| `<string>` | `std::string`, `.find()`, `.substr()`, operators | Week 6 |
| `<vector>` | `vector<T>`, `.push_back()`, `.size()`, `.resize()`, `.insert()` | Week 6 |
| `<ctype.h>` / `<cctype>` | `toupper`, `tolower`, `isalpha`, `isdigit` | Week 5 |
| `<stdlib.h>` / `<cstdlib>` | `malloc`, `free`, `atoi`, `atof` | Week 6 |
| `<memory.h>` | `memcpy`, `memset`, `memmove` | Week 6 |

## Caro Project Specific (Windows Console)

| Library | Purpose |
|---|---|
| `<conio.h>` | `getch()` — non-blocking keyboard input |
| `<windows.h>` | `GetConsoleWindow()`, `SetConsoleCursorPosition()`, `COORD`, `HANDLE` |

## Quality Tools (installed on dev machine)

| Tool | Command | Purpose |
|---|---|---|
| **lizard** | `lizard file.cpp` | Cyclomatic complexity, NLOC, parameter count. Threshold: CCN > 15 |
| **cppcheck** | `cppcheck file.cpp` | Static analysis (bugs, style, unused code) |
| **clang-tidy** | `clang-tidy file.cpp` | Linting, modernization, readability |
| **cpplint** | `cpplint file.cpp` | Google C++ style guide compliance |

Currently installed: **lizard** (via `uv tool install lizard`)

## Compilation

```bash
# Standard homework compilation
g++ -std=c++14 -o program file.cpp

# With coverage (for testing)
g++ -std=c++14 --coverage -o /tmp/prog file.cpp

# Teacher uses Visual Studio 2019 — code must compile there too
```
