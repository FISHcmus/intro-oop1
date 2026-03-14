# Teacher's Expected Code Style

## Key Patterns

| Pattern | Teacher uses | What to do |
|---|---|---|
| I/O | `printf`/`scanf` AND `cout`/`cin` | Both — match exercise context |
| Structs | `struct X {...}; typedef struct X X;` | This exact pattern |
| Arrays | `int a[N]` with `#define N 50` | Static arrays with `#define` |
| Strings | `char[]` with `string.h` AND `std::string` | C-strings for structs, `string` for STL |
| Memory | `new`/`delete` AND `malloc`/`free` | Prefer `new`/`delete` |
| Main function | `void main()` (teacher's habit) | Use `int main()` — `void main()` is non-standard, g++ rejects it |
| Pass-by-ref | `&` references for output params | Always |
| Naming | Vietnamese: `Nhap`, `Xuat`, `SINHVIEN`, `PHANSO` | Match this style |

## Philosophy

- **Use all semester techniques freely** — don't limit to what's been taught that week
- **But stay within what the teacher knows** — no advanced C++ (concepts, ranges, constexpr if, modules, etc.)
- **Clang-tidy > teacher's patterns** — always fix clang-tidy warnings even if teacher's slides ignore them
- Split into small functions for readability, even for simple exercises
- Always handle all edge cases
