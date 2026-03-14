# OOP1 Course Full Analysis

## Course Structure (10 weeks + Final Project)

| Week | Topic | Slides | Weight |
|---|---|---|---|
| 1 | Intro, data types, I/O, basic operators | 33 | Foundation |
| 2 | Control structures, functions, multi-file organization | 61 | Foundation |
| 3 | Algorithms, complexity (O notation), branching strategy | 54 | Core theory |
| 4 | 1D/2D arrays, iteration techniques | 128 | **Critical** |
| 5 | Structs (`typedef`), operator overloading, file I/O | 63 | **Critical** |
| 6 | Pointers P1: `vector<T>`, pointer arithmetic, dynamic allocation | 111 | **Critical** |
| 7 | Pointers P2: 2D dynamic arrays, structural memory, operator `[]` | 132 | **Critical** |
| 8 | Linked lists, stack, queue, abstract data structures | 125 | **Critical** |
| 9 | String processing (C-style + STL `string`), recursion | 87 | Important |
| 10 | Search: linear & binary in arrays, structs, linked lists, 2D | 26 | Important |

## Week-by-Week Summary

### Week 1: Introduction & Basic Operators (33 slides)
- Course overview, grading, Visual Studio 2019
- What is a program: machine code, assembly, high-level languages
- Compilers vs interpreters, compilation process
- IDE (Integrated Development Environment)
- First programs: Hello World, sum, scanf/printf I/O
- C vs C++: `stdio.h`/`printf` vs `iostream`/`cout`
- Data types: `char`, `int`, `long`, `float`, `double`, `sizeof`
- Constants (`#define`, `const`), variables, naming conventions
- Memory layout: stack, heap, BSS, initialized data, text

### Week 2: Control Structure & Source Organization (61 slides)
- **Control structures:** blocks, scope, local/global variables, namespaces
- **Branching:** `if`, `if-else`, `switch-case`
- **Loops:** `while`, `do-while`, `for`
- `break` & `continue`, loop termination conditions
- **Functions:** declaration, definition, calling
- Pass-by-value vs pass-by-reference (`&`)
- Local variables, static local variables
- Input/output/intermediate objects in function design
- Examples: quadratic equation, prime check, prime listing
- Function overloading, default arguments
- Template functions (`template <class T>`)
- Function pointers (passing functions as arguments)
- Multi-file organization: `.h` (interface) + `.cpp` (implementation), `#ifndef` guards
- `static` (file-private) and `extern` (cross-file) keywords

### Week 3: Algorithm & Branching Strategy (54 slides)
- Algorithm definition: limitation, determination, input/output, efficiency, popularity
- Description methods: natural language, flowchart, pseudo-code, programming language
- **Correctness** and **complexity** (time & space)
- Fibonacci: iterative O(n) vs recursive O(2^n) — dramatic time comparison table
- **Linear search** O(n) vs **binary search** O(log n)
- Sentinel technique, flag technique
- **Decision tables:** systematically enumerate all cases before coding
- Date problems: leap year, days in month, next day
- Electricity billing: tiered pricing with progressive thresholds
- Water billing: tiered by household members
- Recursion for branching (n-th root with negative exponents)
- Replacing branches with boolean arithmetic formulas
- Ternary operator
- Building test-case tables for verification

### Week 4: Array & Iteration Technique (128 slides — heaviest)
- **1D arrays:** declaration, indexing, input/output
- 1D array as function parameter (`int a[]`, `int &n`)
- Techniques: sum, count, min/max, search, filter, transform
- Sorting (selection sort, bubble sort, insertion sort)
- **2D arrays:** declaration, row/column traversal
- 2D array as function parameter
- Matrix operations: transpose, multiply, diagonal operations
- **String as char array:** declaration, I/O, basic operations
- **Iteration techniques:** enumeration, generation, permutation

### Week 5: Structural Data & File (63 slides)
- **Structs:** `struct X {...}; typedef struct X X;`
- Nested structs (POINT inside TRIANGLE)
- Struct with pointers (linked nodes, binary trees)
- Bit fields
- **Operator overloading** for structs (`+`, `-`, `+=`)
- Struct arrays, input/output functions (`Nhap`/`Xuat`)
- **File I/O:** `fopen`, `fclose`, `fgetc`, `fgets`, `fscanf`, `fprintf`
- `fputc`, `fputs`, `fflush`
- Binary files: `fread`, `fwrite`, `"rb"`/`"wb"` modes
- `fseek`, `ftell`, `rewind` for file positioning
- Applications: word counting, line counting, pattern search, character frequency
- Structural data in files (PUPIL records)

### Week 6: Pointer P1 (111 slides)
- `vector<T>` from STL: resize, push_back, iterators
- Pointer basics: address-of (`&`), dereference (`*`)
- `string` datatype (STL)
- Memory functions: `new`/`delete`, `malloc`/`free`
- Pointer arithmetic: `p++`, `p+n`, `p-q`
- Pointer techniques: array traversal via pointer
- Heap vs stack memory
- `memcpy`, `memset`, `memmove`
- Dynamic 1D array allocation
- Pointer-checking techniques (null checks, bounds)

### Week 7: Pointer P2 (132 slides)
- Fixed-row 2D arrays (array of pointers)
- Fixed-column 2D arrays
- **Dynamic 2D array:** allocate rows, then columns per row
- Dynamic multi-dimensional arrays
- Structural data with pointers
- Structural memory management
- **Operator `[]` overloading** for custom types
- Structural arrays with dynamic memory
- Enhanced pointer techniques

### Week 8: Linked List & Abstract Data Structure (125 slides)
- **Singly linked list:** node structure, head pointer
- Operations: create, insert (head/tail/middle), delete, search, traverse
- **Stack:** LIFO, push, pop (array-based and linked-list-based)
- **Queue:** FIFO, enqueue, dequeue
- **Abstract linked list** (generic/template)
- Abstract queue and stack
- Other structures mentioned: doubly linked list, circular list

### Week 9: String & Recursion (87 slides)
- **String (C-style):** `strlen`, `strcpy`, `strcat`, `strcmp`, `strstr`, `strncmp`
- Token processing: extracting words from strings
- Search in strings
- String manipulation (reverse, trim, replace)
- Multi-byte/extended characters
- **Recursion:** base case, recursive case
- Categories: direct, indirect, tail recursion
- Applications: factorial, Fibonacci, Tower of Hanoi, permutations
- Recursion vs iteration trade-offs
- Backtracking introduction

### Week 10: Search Problem (26 slides)
- Search space, constraint conditions, optimization standards
- **Linear search** in: 1D array, structural array, vector, linked list, hierarchical structures (folder tree)
- Sentinel technique for linear search
- **Binary search** in: sorted 1D array, unimodal array, structural array (books, cameras, contacts)
- Binary insert maintaining sorted order
- **2D binary search:** row-by-row O(m*log n), staircase O(m+n)
