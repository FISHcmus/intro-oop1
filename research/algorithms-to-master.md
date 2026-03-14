# Algorithms to Master for OOP1

## Tier 1: Recurring Across Multiple Weeks (must know cold)

### Prime Number Check
Used in weeks 2, 3, 5, 10. Teacher's pattern:
```c
int isPrime(long n){
  if(n < 2) return 0;
  for(long i = 2; i <= sqrt(n); i++)
    if(n % i == 0) return 0;
  return 1;
}
```

### GCD (Greatest Common Divisor)
Used for fraction reduction (week 5 PHANSO operations).

### Leap Year / Days in Month / Next Day
Week 3 core. Decision table approach:
- Leap: `(y%4==0 && y%100!=0) || (y%400==0)`
- Days: switch on month, special case Feb + leap check
- Next day: 3 cases (mid-month, end-month, Dec 31)

### Linear Search with Sentinel/Flag
Weeks 3, 4, 10. Two techniques:
- **Sentinel**: add target at end, loop without bounds check
- **Flag**: `bool found = false`, set true when condition met

### Binary Search
Weeks 3, 10. Variations:
- Standard (find exact value in sorted array)
- Find insertion point (maintain sorted order)
- Unimodal array (find peak)
- 2D staircase search O(m+n)

### Fibonacci (iterative vs recursive)
Week 3 core example. Know the time comparison:
- Iterative: O(n) — nanoseconds even for n=200
- Recursive: O(2^n) — 4×10^13 years for n=200

### Tiered Pricing (electricity/water)
Week 3. Progressive thresholds with cumulative computation.
Teacher's improvement: generic `Tinh(L, R, P, k)` range function.

## Tier 2: Data Structure Algorithms (weeks 6-8)

### Dynamic Array Management
- Allocate: `int* a = new int[n];`
- 2D allocate: array of pointers, each `new int[cols]`
- Always `delete[]` after use

### Linked List Operations
- Create node: `NODE* p = new NODE; p->data = x; p->next = NULL;`
- Insert at head, tail, middle (after given node)
- Delete node (head, tail, by value)
- Traverse and search

### Stack (LIFO)
- Array-based: `top` index, push increments, pop decrements
- Linked-list-based: push = insert at head, pop = remove head

### Queue (FIFO)
- Array-based: `front`/`rear` indices (circular)
- Linked-list-based: enqueue at tail, dequeue from head

## Tier 3: String & Recursion (week 9)

### C-String Functions
- `strlen(s)`, `strcpy(dest, src)`, `strcat(dest, src)`
- `strcmp(s1, s2)` — returns 0 if equal, <0 or >0 otherwise
- `strstr(haystack, needle)` — find substring
- `strncmp(s1, s2, n)` — compare first n chars

### Recursion Patterns
- Base case + recursive case
- Factorial: `f(0)=1, f(n)=n*f(n-1)`
- Tower of Hanoi
- Permutation generation
- Tree traversal (folder structure in week 10)

## Tier 4: File I/O (week 5)

### Core Pattern
```c
FILE* fp = fopen("file.txt", "rt");
if(fp == NULL) return;
// ... read/write ...
fclose(fp);
```

### Reading
- `fgetc(fp)` — one character
- `fgets(buf, size, fp)` — one line
- `fscanf(fp, "%d", &x)` — formatted

### Writing
- `fputc(ch, fp)` — one character
- `fputs(str, fp)` — string
- `fprintf(fp, "%d", x)` — formatted

### Positioning
- `fseek(fp, offset, SEEK_SET/SEEK_CUR/SEEK_END)`
- `ftell(fp)` — current position
- `rewind(fp)` — back to start
