# Week 04: Array & Iteration Technique

> **Source**: CSLTr_Week04.ppsx (128 slides)
> **Advisor**: Truong Toan Thinh
> **Note**: Extracted from PPSX XML. Images extracted to `week04_images/`. Diagrams are composed from individual icons -- described in text where possible.

---

## Slide 1 — Title

ARRAY & ITERATION TECHNIQUE
Fundamentals of programming -- Co so lap trinh
Advisor: Truong Toan Thinh

---

## Slide 2 — Contents

- ARRAY
  - One-dimensional array
  - 1D array parameter
  - Techniques in 1D array
  - Two-dimensional array
  - 2D array parameter
  - Techniques in 2D array
  - String
- ITERATION TECHNIQUE

---

## Slide 3 — One-Dimensional Array

A collection of all elements with the same datatype.

> *[Visual: Array named `a`, has 9 elements, index from 0 to 8]*

| Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|-------|---|---|---|---|---|---|---|---|---|
| Address | <100> | <102> | <104> | <106> | <108> | <110> | <112> | <114> | <116> |
| Value | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |

Syntax getting the values: `a[<index>]`, e.g., `a[0]` returns 9.

---

## Slide 4 — One-Dimensional Array: Declaration

To create a static array, we need:
- Datatype for each element
- Name for 1D array variable
- A number of elements in array (array size)

Syntax: `<Datatype> name[size]`

Examples:
- `int thang[12]`: Array named 'thang' containing 12 elements with `int`
- `double a[5]`: Array named 'a' containing 5 elements with `double`

---

## Slide 5 — One-Dimensional Array: Example

```c
#include <stdio.h>
#define N 50
void arrayIntInput(int a[N], int& n){
  while(1){
    printf("Nhap so phan tu can dung < %d", N);
    scanf("%d", &n);
    if(n < 0 || n > N) printf("Xin nhap lai\n");
    else break;
  }
  for(int i = 0; i < n; i++){
    printf("Nhap a[%d]: ", i); scanf("%d", &a[i]);
  }}
```

---

## Slide 6 — One-Dimensional Array: Indexing & Assignment

- Index is either constant or expression
  ```c
  int a[10], x = 2, y = 3;
  a[(x + y)/2 + 1] = 5; //a[3] = 5
  ```

- Cannot assign array to array
  ```c
  #define N 50
  int a[N], b[N];
  a = b; //Wrong
  ```

- Using loop to assign each element:
  ```c
  for(int i = 0; i < N; i++)
    a[i] = b[i];
  ```

---

## Slide 7 — One-Dimensional Array: Input

- Separately input value to each element (not recommended):
  ```c
  int a[2];
  scanf("%d", &a[0]);
  scanf("%d", &a[1]);
  ```

- Should use loop to input value:
  ```c
  int a[2];
  for(int i = 0; i < 2; i++)
    scanf("%d", &a[i]);
  ```

---

## Slide 8 — One-Dimensional Array: Output

- Separately output value (not recommended):
  ```c
  printf("%d", a[0]);
  printf("%d", a[1]);
  ```

- Should use loop to output value:
  ```c
  for(int i = 0; i < 2; i++)
    printf("%d", a[i]);
  ```

---

## Slide 9 — One-Dimensional Array: Initialization

Some ways to initialize 1D array:

```c
int a[5] = {1,2,3,4,5};
int a[5] = {1,2,3};       // remaining elements are 0
int a[5] = {0};            // all elements are 0
int a[] = {1,2,3};         // size is 3
```

Using operator `sizeof` to determine array size:

```c
int a[] = {1,2,3}, n;
n = sizeof(a)/sizeof(a[0]);
for(int i = 0; i < n; i++)
  printf("%d", a[i]);
```

---

## Slide 10 — 1D Array Parameter

Transmit 1D array to function:
- Once sending 1D array with N elements to function, we need only transmit the first element's address
- The first element's address can be represented with symbol `&a[0]` or `a`.
- Example: we have a static 1D array named 'a', so 'a' is a **constant address**.
- Note: 'a' is the address of `a[0]` without symbol '&'.

---

## Slide 11 — 1D Array Parameter: Example

```c
#include <stdio.h>
#define N 50
void arrayIntInput(int a[N], int& n){...}
void arrayIntOutput(int a[N], int n){
  for(int i = 0; i < n; i++)
    printf("%d", a[i]);
}
void main(){
  int b[N], m;
  arrayIntInput(b, m);
  arrayIntOutput(b, m);
}
```

> Note: `&b != b = &b[0]`, `&a != a = &a[0]`, `&n == &m`

---

## Slide 12 — Techniques in 1D Array: Exchange Sort

Sort -- Exchange sort algorithm:
- Simple, easy to write
- Inefficient with big array size
- Suitable for demonstration

```c
void Sapxep(int a[], int n){
  for(int i = 0; i < n - 1; i++){
    for(int j = i + 1; j < n; j++){
      if(a[i] > a[j]){
        int temp = a[i];
        a[i] = a[j]; a[j] = temp;
      }
    }
  }}
```

---

## Slides 13--14 — Techniques in 1D Array: Sort Trace

> *[Visual: Step-by-step trace of exchange sort on array {9,6,3,5,8,2,1,4}]*

**i = 0**: Compares `a[0]` with each `a[j]` (j=1..7), swapping when `a[0] > a[j]`.
Result after i=0: `{1, 9, 6, 5, 8, 3, 2, 4}`

**i = 1**: Compares `a[1]` with each `a[j]` (j=2..7).
...

**i = 6**: Final pass. Result: `{1, 2, 3, 4, 5, 6, 8, 9}`

---

## Slide 15 — Techniques in 1D Array: Insert Element

Add a whole number `x` into `n`-sized array at position `k`.

```c
bool Chen(int a[], int &n, int x, int k){
  if( k < 0 || k > n) return false;
  for(int i = n - 1; i >= k; i--)
    a[i+1] = a[i];
  a[k] = x;
  n++;
  return true;
}
```

---

## Slide 16 — Techniques in 1D Array: Erase Element

**Erase with order maintenance:**

```c
void Xoa(int a[], int &n, int k){
  if( k < 0 || k > n) return;
  for(int i = k; i < n - 1; i++)
    a[i] = a[i + 1];
  n--;
}
```

**Erase without maintaining order:**

```c
void Xoa(int a[], int &n, int k){
  if( k < 0 || k > n) return;
  a[k] = a[n - 1];
  n--;
}
```

---

## Slide 17 — Techniques in 1D Array: Split (Even/Odd)

Divide data of 1 array into other 2 arrays (even & odd elements):

```c
void Tachchanle(int a[], int n, int c[], int &k, int l[], int &h){
  k = h = 0;
  for(int i = 0; i < n; i++){
    if(a[i] % 2 == 0) c[k++] = a[i];
    else l[h++] = a[i];
  }
}
```

---

## Slide 18 — Techniques in 1D Array: Split in Half

Separate data of one array into two halves:

```c
void Tachdoi(int a[], int n, int b[], int &k, int c[], int &h){
  k = h = 0;
  for(int i = 0; i < n/2; i++)
    b[k++] = a[i];
  for(int j = n/2; j < n; j++)
    c[h++] = a[j];
}
```

---

## Slide 19 — Techniques in 1D Array: Concatenate

Unify data of 2 arrays into 1 array:

```c
void GhepNoi(int a[], int &n, int b[], int k, int c[], int h){
  n = 0;
  for(int i = 0; i < k; i++)
    a[n++] = b[i];
  for(int j = 0; j < h; j++)
    a[n++] = c[j];
}
```

---

## Slide 20 — Techniques in 1D Array: Alternate Merge

Alternate data of 2 arrays into 1 array:

```c
void GhepXK(int a[], int &n, int b[], int k, int c[], int h){
  n = 0; int i = 0, j = 0;
  while((i < k) && (j < h)){
    if(n % 2 == 0) a[n++] = b[i++];
    else a[n++] = c[j++];
  }
  while(i < k) a[n++] = b[i++];
  while(j < h) a[n++] = c[j++];
}
```

---

## Slide 21 — Two-Dimensional Array

Matrix (table) has elements with the same datatype.

> *[Visual: Matrix named 'a', has 9 elements (3x3)]*

|       | Col 0 | Col 1 | Col 2 |
|-------|-------|-------|-------|
| Row 0 | 9     | 8     | 7     |
| Row 1 | 6     | 5     | 4     |
| Row 2 | 3     | 2     | 1     |

Syntax getting values: `a[<row-index>][<column-index>]`, for example, `a[0][1]` returns 8.

---

## Slide 22 — Two-Dimensional Array: Declaration

To create a static matrix, we need:
- Datatype for each element
- Name for matrix
- Row and Column of matrix (matrix size)

Syntax: `<Datatype> name[row-size][column-size]`

Examples:
- `int a[12][11]`: matrix 'a' contains 12 x 11 elements of `int`
- `double a[5][5]`: matrix 'a' contains 5 x 5 elements of `double`

---

## Slide 23 — Two-Dimensional Array: Input Example

```c
#include <stdio.h>
#define MaxRow 20
#define MaxCol 20
void main(){
  int a[MaxRow][MaxCol], mRow, nCol, i, j;
  printf("mRow = "); scanf("%d", &mRow);
  printf("nCol = "); scanf("%d", &nCol);
  for(i = 0; i < mRow; i++){
    for(j = 0; j < nCol; j++)
      scanf("%d", &a[i][j]);
  }
}
```

---

## Slide 24 — Two-Dimensional Array: Indexing & Assignment

- Index is either constant or expression:
  ```c
  int a[7][7], x = 2, y = 3;
  a[y - x][y + x] = 5; //a[1][5] = 5
  ```

- Cannot directly assign matrix to matrix:
  ```c
  a = b; //Wrong
  ```

- Using loop to assign:
  ```c
  for(int i = 0; i < R; i++)
    for(int j = 0; j < C; j++)
      a[i][j] = b[i][j];
  ```

---

## Slides 25--26 — Two-Dimensional Array: Input/Output

**Input** -- use nested loops:
```c
for(int i = 0; i < 2; i++)
  for(int j = 0; j < 2; j++)
    scanf("%d", &a[i][j]);
```

**Output** -- use nested loops:
```c
for(int i = 0; i < 2; i++)
  for(int j = 0; j < 2; j++)
    printf("%d", a[i][j]);
```

---

## Slide 27 — Two-Dimensional Array: Initialization

```c
int a[10][2] = {{1,2}, {2,4}, {3,6}, {4,7},{8,9}, {2,1}, {6,5}, {3,8}, {4,9}, {1,1}};
int a[10][2] = {1, 2, 2, 4, 3, 6, 4, 7, 8, 9,
                2, 1, 6, 5, 3, 8, 4, 9, 1, 1};
int a[3][2] = {1,2,3,4,5};
//remaining elements are automatically assigned zero
```

---

## Slide 28 — 2D Array Parameter

Transmit matrix to function:
- Only sending address at caller
- In the area of function declaration & definition, only explicitly determine column size
- C functions 2D array as 1D array of 1D array

```c
#define M 2
#define N 3
void XuLy(int a[][N], int m, int n){
  //...
}
void main(){
  int a[M][N], m, n;
  //...
  XuLy(a, m, n);
  //a[i][j] = *(*(a + i) + j) => 4 = *((*a) + 3) = *(*(a+1))
}
```

---

## Slides 29--30 — 2D Array Parameter: Input/Output Functions

```c
#include <stdio.h>
#define M 20
#define N 30
void array2DIntInput(int b[][N], int& m, int& n){
  scanf("%d", &m); // Nhap so dong
  scanf("%d", &n); // Nhap so cot
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      scanf("%d", &b[i][j]);
    }
  }
}
void array2DOutput(int a[][N], int m, int n){
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++)
      printf("%d", a[i][j]);
    printf("\n");
  }
}
void main(){
  int a[M][N], m, n;
  array2DInput(a, m, n);
  array2DOutput(a, m, n);
}
```

---

## Slide 31 — Techniques in 2D Array: Ascending Sort

```c
void sapxeptang(int a[][20], int d, int c) {
  for (int i=0; i<d*c; i++) {
    for (int j=0; j<d*c; j++) {
      if (a[i/c][i%c] < a[j/c][j%c]) {
        int tmp = a[i/c][i%c];
        a[i/c][i%c] = a[j/c][j%c];
        a[j/c][j%c] = tmp;
      }
    }
  }
}
```

Example: `{{2,6},{1,5},{8,9}}` -> `{{1,2},{5,6},{8,9}}`

---

## Slide 32 — Techniques in 2D Array: Copy Row

Copy row k into row h:

```c
void ChepDong(int a[][20], int m, int n, int k, int h) {
  if(k<1 || k >= m || h < 1 || h >= m) return;
  for (int i = 0; i<n; i++) {
    a[h][i] = a[k][i];
  }
}
```

Example (copy row 0 into row 2):
```
2 6 1 2      2 6 1 2
1 5 8 9  ->  1 5 8 9
1 4 6 7      2 6 1 2
```

---

## Slide 33 — Techniques in 2D Array: Copy Column

Copy column k into column h:

```c
void ChepCot(int a[][20], int m, int n, int k, int h) {
  if(k<1 || k >= n || h < 1 || h >= n) return;
  for (int i = 0; i<m; i++) {
    a[i][h] = a[i][k];
  }
}
```

Example (copy column 0 into column 3):
```
2 6 1 2      2 6 1 2
1 5 8 9  ->  1 5 8 1
1 4 6 7      1 4 6 1
```

---

## Slide 34 — Techniques in 2D Array: Permute Rows

Permute row k and row h:

```c
void HoanViDong(int a[][20], int m, int n, int k, int h) {
  int i, temp;
  if(k == h) return;
  for (i = 0; i < n; i++) {
    temp = a[k][i];
    a[k][i] = a[h][i];
    a[h][i] = temp;
  }
}
```

Example (permute row 0 and row 2):
```
2 6 1 2      1 4 6 7
1 5 8 9  ->  1 5 8 9
1 4 6 7      2 6 1 2
```

---

## Slide 35 — Techniques in 2D Array: Permute Columns

Permute column k and column h:

```c
void HoanViCot(int a[][20], int m, int n, int k, int h) {
  int i, temp;
  if(k == h) return;
  for (i = 0; i < m; i++) {
    temp = a[i][k];
    a[i][k] = a[i][h];
    a[i][h] = temp;
  }
}
```

Example (permute column 0 and column 3):
```
2 6 1 2      2 6 1 2
1 5 8 9  ->  9 5 8 1
1 4 6 7      7 4 6 1
```

---

## Slide 36 — Techniques in 2D Array: Remove Row (No Order)

Remove row k (without maintaining order):

```c
void LoaiBoDong(int a[][20], int &m, int n, int k) {
  if((k < 0) || (k >= m)) return;
  if(k != m - 1){
    ChepDong(a, m, n, m - 1, k);
    m--;
  }
}
```

Example (remove row 0):
```
2 6 1 2      1 4 6 7
1 5 8 9  ->  1 5 8 9
1 4 6 7
```

---

## Slides 37--38 — Techniques in 2D Array: Remove Row (Order Maintained)

**Method 1**: Using temporary matrix:

```c
void LoaiBoDong(int a[][20], int &m, int n, int k) {
  if((k < 0) || (k >= m)) return;
  if(k == m - 1){ m--; return; }
  int b[20][20], t = 0;
  for(int i = 0; i < m; i++){
    if(i == k) continue;
    for(int j = 0; j < n; j++) b[t][j] = a[i][j];
    t++;
  }
  for(int i = 0; i < t; i++)
    for(int j = 0; j < n; j++)
      a[i][j] = b[i][j];
  m--;
}
```

**Method 2**: Shifting rows up:

```c
void LoaiBoDong(int a[][20], int &m, int n, int k) {
  if((k < 0) || (k >= m)) return;
  for(int i = k; i < m - 1; i++) {
    for(int j = 0; j < n; j++) {
      a[i][j] = a[i + 1][j];
    }
  }
  m--;
}
```

---

## Slide 39 — Techniques in 2D Array: Remove Row (Using Permute)

**Method 3**: Using HoanViDong to roll down the removed row:

```c
void LoaiBoDong(int a[][20], int &m, int n, int k) {
  if((k < 0) || (k >= m)) return;
  for(int i = k; i < m - 1; i++) {
    HoanViDong(a, m, n, i, i + 1);
  }
  m--;
}
```

---

## Slide 40 — String

- A collection of elements with `char`
- Finishes with character `'\0'`

Declaration and definition:
```c
char s[] = "Nguyen Van A";
// Character array, named s, has 13 elements (12 content + '\0')

char s[] = {'h', 'e', 'l', 'l', 'o'};
```

Input/Output:
```c
char s[30];
gets_s(s);   // remove '\n' out of stdin & assign '\0' at the end of s
printf("%s\n", s);
```

---

## Slide 41 — String: strlen & strcat

Convenient functions in `<string.h>`:

**`strlen(char*)`**: return number of characters in string
```c
char s[30];
gets_s(s);
printf("%d", strlen(s));
```

**`strcat(char* dest, char* src)`**: join two strings `dest` and `src` into one, return the address of string `dest`.
```c
char s[30], t[30], *kq;
gets_s(s); gets_s(t);
kq = strcat(s, t);
printf("%s", kq);
```

---

## Slide 42 — String: strchr & strstr

**`strchr(const char* src, int ch)`**: return a pointer to the first position of string `src` containing character `ch`
```c
// Pointer to constant: cannot change value
const char *s = "He world";
// constant pointer (char* const s): can change value
int ch = 101;
char* c = strchr(s, ch);
if(c != NULL) printf("%c", *c);
```

**`strstr(const char* mainStr, const char* subStr)`**: return a pointer to the first position of substring `subStr` belonging to `mainStr`.
```c
char *s = "hello world", *t = "world";
char *kq = strstr(s, t);
if (kq!=NULL) printf("%s", kq);
```

---

## Slide 43 — String: strtok

**`strtok(char* s, char* delim)`**: separate 'word' of string s (based on `delim`). Once invoked, this function returns a pointer to 'word' just separated.

```c
char *s = "They are dogs, cats. The dogs";
char *sep = " ,.";
char* w = strtok(s, sep);
while(w!=NULL){
  printf("%s\n", w);
  w = strtok(NULL, sep);
}
```

---

## Slide 44 — String: Pointer to Constant & Constant Pointer

**Pointer to constant**: `const char* s;`
```c
char m = 'M', n = 'N';
const char* s = &m;
*s = n;  // Error
s = &n;  // OK
```

**Constant pointer**: `char* const s;`
```c
char m = 'M', n = 'N';
char* const s = &m;
*s = n;  // OK
s = &n;  // Error
```

---

## Slide 45 — Iteration Technique: Contents

- Some basic iteration algorithms
- Stop condition
- Prime number verification
- Fast exponentiation
- Number factorization
- Iteration techniques
- Loop techniques in array
- Recursion

---

## Slides 46--47 — Sum/Product Algorithm

- Var `sum` is initialized with 0
- Var `product` is initialized with 1
- Scan all elements in a collection (array)
- Accumulate into var sum/product

```c
// Method 01
void TinhTongTich(long n, long& s, long& p){
  s = 0, p = 1;
  for(int i = 1; i < n; i++){
    s+=i;
    p*=i;
  }
}

// Method 02
void TinhTongTich(long n, long& s, long& p){
  s = 0, p = 1;
  for(; n > 0; n--){
    s+=n;
    p*=n;
  }
}
```

---

## Slide 48 — Sum/Product with Conditions

Given n in Z+, compute:
- Sum `s` of all numbers with the form m/(1 + m^2), where m % 4 = 1 & m < n
- Product `p` of all odd m, where m % 3 = 1 & m < n

```c
void main(){
  long m, n; double s = 0, p = 1;
  scanf("%d", &n); m = n;
  for(; m > 0; m--){
    if(m % 4 == 1) s+=m/(1.0+m*m);
    if(m % 2 == 1 && m% 3 == 1) p*=m;
  }
  printf("%lf\n", s); printf("%lf\n", p);
}
```

---

## Slide 49 — Compute Series: x/1 + x^2/2 + ... + x^n/n

Note: x^2 = x * x, x^3 = x * x^2, ..., x^n = x * x^(n-1)

```c
double TinhTong(double x, int n){
  double s = 0, p = 1;
  for(int i = 1; i <= n; i++){
    p*=x;
    s+=(p/i);
  }
  return s;
}
void main(){
  double x, s = 0; int n;
  scanf("%lf", &x); scanf("%d", &n);
  s = TinhTong(x, n);
  printf("%lf", s);
}
```

---

## Slides 50--51 — Compute Alternating Series: -x/1 + x^2/2 - x^3/3 + ...

**Method 1**: Change p = -1 & transmit -x in main function:

```c
double TinhTong(double x, int n){
  double s = 0, p = -1;
  for(int i = 1; i <= n; i++){
    p*=x;
    s+=(p/i);
  }
  return s;
}
// In main: s = TinhTong(-x, n);
```

**Method 2**: Change the way of calling function in main:

```c
// Same TinhTong with p = 1
// In main: s = -TinhTong(-x, n);
```

---

## Slides 52--53 — Compute K_n(x) Using Recurrence

Rewrite K_n(x) by replacing x with (-x) to simplify sign (+/-) problem.

To compute i-th component, take previous result and multiply by (x/i):

```c
double TinhK(double x, int n){
  double s = 0, p = 1;
  for(int i = 1; i <= n; i++){
    p*=(x/i); s+=p;
  }
  return s;
}
void main(){
  double x, s, n;
  scanf("%f", &x); scanf("%d", &n);
  s = TinhK(-x, n);
}
```

---

## Slide 54 — Integer Square Root

Formula: 1 + 3 + ... + (2t - 1) = t^2.
Find t such that t^2 <= n.

```c
double sqrtInt(long n){
  long s = 1, k = 1;
  while(s < n){
    k += 2; s += k;
  }
  if(s > n) k = k - 2;
  return (k + 1)/2;
}
```

---

## Slides 55--56 — Perfect Numbers

- 6 is a perfect number: 6 = 1 + 2 + 3
- 28 is a perfect number: 28 = 1 + 2 + 4 + 7 + 14
- All perfect numbers have the form 2^(k-1) * (2^k - 1)

```c
long sumDivisors(long n){
  long s = 0, k = n;
  while(--k){
    if(n % k) s+=k;
  }
  return s;
}
int isPerfect(long n){
  if(n == sumDivisors(n)) return 1;
  return 0;
}
void main(){
  long m;
  scanf("%ld", &m);
  while(m--){
    if(isPerfect(m))
      printf("%ld\n", m);
  }
}
```

---

## Slides 57--58 — Count Algorithm

- Var `dem` (count) is initialized with 0
- Scan all elements in an array
- If element satisfies the condition, increase `dem` by one

Count divisors of whole number n > 0:

```c
int DemUocSo(long n){
  long count = 0;
  for(int i = 1; i <= n; i++){
    if(n % i == 0)
      count++;
  }
}
```

---

## Slide 59 — Count Digits/Characters in String

```c
void DemKiSoKiTu(char* s, int& c, int& n){
  c = n = 0;
  for(int i = 0; i < strlen(s); i++){
    if('a' <= s[i] && s[i] <= 'z')
      c++;
    else if('A' <= s[i] && s[i] <= 'Z')
      c++;
    else n++;
  }
}
void main(){
  char s[] = "Hello world";
  int n, c;
  DemKiSoKiTu(s, c, n);
  printf("%d %d", n, c);
}
```

---

## Slide 60 — Count Negative/Positive/Zero

```c
void DemSo(float a[], int n, int* nd, int* na, int* no){
  *nd = *na = *no = 0;
  for(int i = 0; i < n; i++){
    if(a[i] > 0)
      (*nd)++;
    else if(a[i] < 0)
      (*na)++;
    else (*no)++;
  }
}
```

---

## Slides 61--62 — Checking Conditions on Arrays

Given float array `a` and float var `x`:
- Does `x` appear in `a`? -- Count equals, check if positive
- Is `x` bigger than all elements? -- Count smaller, check if == n
- Is `x` smaller than another element? -- Count bigger, check if positive
- Are all elements negative? -- Use `GreaterAll(a, n, 0)`

```c
int Appear(float a[], int n, float x){
  int cnt = 0;
  while(n--){
    if(x == a[n]) cnt++;
  }
  return (cnt > 0);
}

int GreaterAll(float a[], int n, float x){
  int cnt = 0, m = n;
  while(m--){
    if(x > a[m]) cnt++;
  }
  return (cnt == n);
}

int LessOne(float a[], int n, float x){
  int cnt = 0;
  while(n--){
    if(x < a[n]) cnt++;
  }
  return (cnt > 0);
}

int AllNegative(float a[], int n){
  return GreaterAll(a, n, 0);
}
```

---

## Slide 63 — Check Ascending/Descending

Is array `a` ascending? a[i-1] <= a[i] for all i in [1, n-1].

```c
int isIncreasing(float a[], int n){
  int cnt = 0, m = n;
  while(--m){
    if(a[m - 1] <= a[m]) cnt++;
  }
  return (cnt == n - 1);
}
```

---

## Slides 64--66 — Find Min/Max Element

**Algorithm**: Choose x0 as expected value, scan remaining, update if better found.

**Find smallest element:**

```c
int PhanTuNhoNhat(float a[], int n){
  int idx = 0;
  for(int i = 0; i < n; i++){
    if(a[idx] > a[i])
      idx = i;
  }
  return idx;
}
```

**Find smallest POSITIVE element (with flag):**

```c
int PhanTuDuongNhoNhat(float a[], int n){
  int idx = -1;
  for(int i = 0; i < n; i++){
    if(a[i] > 0){
      if(idx == -1 || a[idx] > a[i]){
        idx = i;
      }
    }
  }
  return idx;
}
```

---

## Slide 67 — Find Element != 0 with Smallest Absolute Value

```c
int absMinNotZero(float a[], int n){
  int idx = -1;
  for(int i = 0; i < n; i++){
    if(a[i] != 0)
      if(idx == -1 || fabs(a[idx]) > fabs(a[i]))
        idx = i;
  }
  return idx;
}
```

---

## Slide 68 — Find Pair with Smallest Difference

Find pair a[i] & a[j] such that a[i] != a[j] & |a[i] - a[j]| has the smallest value:

```c
double minPair(double a[], int n, int& idi, int& idj){
  double dmin = -1, d; idi = idj = -1;
  for(int j = n - 1; j >= 1; j--){
    for(int i = j - 1; i >= 0; i--)
      if(a[i] != a[j]){
        d = fabs(a[i] - a[j]);
        if(dmin == -1 || dmin > d){
          dmin = d; idi = i; idj = j;
        }
      }
  }
  return dmin;
}
```

---

## Slides 69--70 — Maximum Subarray Sum

Find pair of indexes (i, k) such that 0 <= i <= k < n & a[i] + ... + a[k] has the biggest value.

**Basic version** (O(n^3)):
```c
float maxS(float a[], int n, int& Idi, int& Idk){
  float lc = a[0]; Idi = Idk = 0;
  for(int i = 0; i < n; i++)
    for(int k = i; k < n; k++){
      float s = 0;
      for(int j = i; j <= k; j++) s+=a[j];
      if(s > lc){
        lc = s; Idi = i; Idk = k;
      }
    }
  return lc;
}
```

**Improved version** (O(n^2)):
```c
float maxS(float a[], int n, int& Idi, int& Idk){
  float lc = a[0]; Idi = Idk = 0;
  for(int i = 0; i < n; i++){
    float s = 0;
    for(int k = i; k < n; k++){
      s+=a[k];
      if(s > lc){
        lc = s; Idi = i; Idk = k;
      }
    }
  }
  return lc;
}
```

---

## Slides 71--73 — Stop Condition / Loop Invariant

'Invariance' of loop is a logic expression that is true before and after each iteration.

**Example 1**: Compute sum = 1/1 + 1/2 + ... + 1/n
```c
double sum = 0;
for(int i = 1; i <= n; i++){
    sum+=1.0/i;
}
```

**Example 2**: Compute x^n
```c
double y = 1;
for(int i = 0; i < n; i++)
    y*=x;
```

---

## Slides 74--76 — Quantifier Verification

- Statement (forall x in S)(P(x)) is true when true for all x.
- Statement (exists x in S)(P(x)) is true if at least one x makes P(x) true.

**Check if x > smallest element** (exists):
```c
int greaterMin(double a[], int n, double x){
  int kt = 0;
  for(int i = 0; i < n; i++){
    if(x > a[i]){
      kt = 1;
      break;
    }
  }
  return kt;
}
```

**Check if x > all elements** (forall):
```c
int greaterMax(double a[], int n, double x){
  int kt = 1;
  for(int i = 0; i < n; i++){
    if(x <= a[i]){
      kt = 0;
      break;
    }
  }
  return kt;
}
```

---

## Slide 77 — Linear Search

Check if there exists x in double n-sized array:

```c
int Find(double a[], int n, double x){
  int vt = -1;
  for(int i = 0; i < n; i++){
    if(x == a[i]){
      vt = i;
      break;
    }
  }
  return vt;
}
```

---

## Slide 78 — Verify Ascending Array

Check if float array is ascending (forall i in {1,...,n-1}: a[i-1] <= a[i]):

```c
int Increasing(double a[], int n){
  int KT = 1;
  for(int i = 1; i < n; i++){
    if(a[i - 1] > a[i]){
      KT = 0;
      break;
    }
  }
  return KT;
}
```

---

## Slides 79--81 — Prime Number Verification

- Prime number has only two divisors
- Number one is not prime
- Check if a > 1 is composite: exists i in {2,...,a-1} such that i | a

**Slow algorithm:**
```c
int isPrime(long a){
  int kq;
  if(a < 0) a = -a;
  switch(a){
    case 0: kq = 1; break;
    case 1: kq = 0; break;
    default:{
      long i = 2; kq = 1;
      while(i < a){
        if(a % i == 0){kq = 0; break;}
        i++;
      }}
  }
  return kq;
}
```

**Optimized algorithm** (check only odd numbers up to sqrt(a)):
```c
int isPrime(long a){
  int kq;
  if(a < 0) a = -a;
  switch(a){
    case 1: kq = 0; break;
    case 0: case 2: case 3: kq = 1; break;
    default:{
      long i = 3;
      if(a % 2 == 0){ kq = 0; break;}
      kq = 1;
      while(i <= sqrt(a)){
        if(a % i == 0){ kq = 0; break;}
        i+=2;
      }
    }
  }
  return kq;
}
```

---

## Slide 82 — Greatest Common Divisor (GCD)

Let a & b (a, b in Z+), d is called the **g**reatest **c**ommon **d**ivisor of a & b if:
- d is the a's divisor & d is the b's divisor
- If there exists t being a's divisor & b's divisor, then t is the d's divisor

**Euclid algorithm:**
```
while(a != 0 && b != 0)
  if a > b then a = a mod b
  else b = b mod a
return a + b;
```

a & b are called coprime if and only if gcd(a, b) = 1.

---

## Slides 83--85 — Fast Exponentiation

Problem: computing a^n.

Improvement from formula:
- If n is even: a^n = (a^(n/2))^2
- If n is odd: a^n = a * (a^((n-1)/2))^2

**Naive algorithm** (O(n)):
```c
double power_n(double a, long n){
  double kq = 1;
  for(int i = 0; i < n; i++){
    kq*=a;
  }
  return kq;
}
```

**Fast algorithm** (O(log n)):
```c
double power_n(double a, long n){
  double kq = 1;
  for(; n > 0; n/=2){
    if(n % 2 == 1) kq*=a;
    a*=a;
  }
  return kq;
}
```

**Trace example**: a = 3 & n = 12
- Start: kq = 1, a = 3, n = 12
- 1st: kq = 1, a = 9, n = 6
- 2nd: kq = 1, a = 81, n = 3
- 3rd: kq = 81, a = 6561, n = 1
- 4th: kq = 81*6561, a = 43046721, n = 0
- Result: kq = 531441

---

## Slides 86--87 — Modular Exponentiation

Problem: a^n mod b. Cannot firstly compute a^n then mod b (because a^n may be large).

Note: a^n mod b = [(a^(n-1) mod b) * a] mod b.

**Fast modular exponentiation:**
```c
long modPow(long a, long b, long n){
  long kq = 1;
  for(; n > 0; n/=2){
    if(n % 2 == 1) kq = kq*a % b;
    a = a*a % b;
  }
  return kq;
}
```

**Trace example**: a = 3, n = 5 & b = 12
- Start: kq = 1, a = 3, n = 5
- 1st: kq = 3, a = 4, n = 2
- 2nd: kq = 3, a = 1, n = 1
- 3rd: kq = 3, a = 1, n = 0
- Result: kq = 3

---

## Slides 88--90 — Number Factorization

Always factorize n = p1^e1 * p2^e2 * ... where pi are prime numbers & ej are natural numbers.

Example: 315 = 3^2 * 5 * 7

**Print version:**
```c
void factors(long n){
  long p = 2, c = 0;
  while(n > 1) {
    if(n % p == 0) {
      c++; n = n/p;
    }
    else{
      if(c > 0) printf("%ld ^ %ld  ", p, c);
      p++; c = 0;
    }
  }
  if(c > 0) printf("%ld ^ %ld", p, c);
}
```

**Storage version (with sqrt optimization):**
```c
void factors(long n, long pr[], long e[], long& np){
  long p = 2, c = 0, np = 0;
  double sqrtN = sqrt(n);
  while(n > 1) {
    if(n % p == 0) {
      if(c == 0){ pr[np] = p; np++; }
      c++; e[np - 1] = c; n/=p;
    }
    else {
      p++; c = 0;
      if(p > sqrtN){
        pr[np] = n; e[np] = 1; np++; break;
      }
    }
  }
}
```

---

## Slide 91 — Fibonacci Iteration

```
F0 = F1 = 1
Fn = Fn-1 + Fn-2 (n >= 2)
```

Pseudocode:
```
Fibo(int n){
  if n = 0 or n = 1 then return 1
  while(k < n)
    F = lastF + F
    lastF = F - lastF
    k = k + 1
}
```

---

## Slide 92 — Recurrence: S_n = x^n + y^n + z^n

Given x^3 + ax^2 + bx + c = 0, compute S_n = x^n + y^n + z^n.

Based on equation's characteristic:
- S0 = 3
- S1 = x + y + z = -a
- S2 = a^2 - 2b
- Sn = -a*S(n-1) - b*S(n-2) - c*S(n-3) (n >= 3)

---

## Slide 93 — Approximate sin(x) & cos(x)

Using identities:
- sin(x) = 2sin(x/2)cos(x/2)
- cos(x) = 1 - 2sin^2(x/2)

If x in [0, 2pi) and n >= 20:
- sin(x/2^n) ~ (x/2^n) - (x/2^n)^3/6
- cos(x/2^n) ~ 1 - (x/2^n)^2/2

Pseudocode:
```
ComputeSinCos(double x, double& sinx, double& cosx)
  Set n >= 20
  Compute x = x - 2*pi (normalize to [0, 2*pi))
  Let x = x/2^n & compute sinx = x - x^3/6, cosx = 1 - x^2/2
  while(n > 0)
    sx = 2*sinx*cosx, cx = 1 - 2*sinx*sinx
    sinx = sx, cosx = cx
    n = n - 1
```

---

## Slide 94 — Square Root Approximation (Newton's Method)

x0 = 1, xn = x(n-1)/2 + a/(2*x(n-1)) for n >= 1

```
#define epsilon 0.0000001
double sqrtR(double a)
  if a <= 0 then return 0
  double xLast, x = 1
  do{
    xLast = x
    x = xLast/2 + a/(2*xLast)
  } while(|x - xLast| >= epsilon)
  return x
```

---

## Slides 95--97 — Loop Techniques: Look-up Table

**Example 1**: Convert number to string using array instead of if/switch:
```c
char* toString(int n){
  char* s[] = {"zero", "one", ..., "ten"}, *res = "Negative";
  if(n >= 0){
    if(0 <= n && n <= 10) res = s[n];
    else if(n <= 15) res = "Eleven to fifteen";
    else res = "Greater than fifteen";
  }
  return res;
}
```

**Example 2**: Computing lunar year (Vietnamese zodiac):
```c
string toString(int y){
  string c[] = {"canh", "tan", "nham", ..., "ky"};  // 10 zodiacs
  string g[] = {"than", "dau", "tuat", ..., "mui"};  // 12 animal designations
  return c[y % 10] + " " + g[y % 12];
}
```

---

## Slide 98 — Look-up Table: Days in Month

```c
int checkLeap(int y){
  return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}
int nDayOfMonth(int m, int y){
  int Days[] = {-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int nDay = -1;
  if(checkLeap(y)) Days[2] = 29;
  nDay = Days[m];
  return nDay;
}
```

---

## Slide 99 — Look-up Table: Electricity Payment

```c
const int _ext = -1;
const int L[] = {0, 100, 150, 200, 300, 400, _ext};
const double P[] = {1242, 1304, 1651, 1788, 1912, 1962};
int nP = sizeof(P)/sizeof(P[0]), nL = sizeof(L)/sizeof(L[0]);

double subCompute(int Lm1, int Lm2, double Prc, int kWh){
  if(kWh >= Lm1){
    if(kWh < Lm2 || Lm2 == _ext) return (kWh - Lm1)*Prc;
    else return (Lm2 - Lm1)*Prc;
  }
  return 0;
}

double TienDien(int kWh){
  double s = 0;
  for(int i = 0; i < nL - 1; i++)
    s+=subCompute(L[i], L[i+1], P[i], kWh);
  return s*1.1; // tax
}
```

---

## Slides 100--101 — Loop Techniques: Interchange Sort

Sort problem "rearranges" the positions of elements with some order.

**Absolute ascending sort** (interchange sort):
```c
void absSort(double a[], int n){
  for(int i = 0; i < n - 1; i++){
    for(int j = i + 1; j < n; j++){
      if(fabs(a[i]) > fabs(a[j])){
        double temp = a[j];
        a[j] = a[i];
        a[i] = temp;
      }
    }
  }
}
```

Example: `{-2.7, 14.5, -5, 8.5, 0.8, 4.5, 8.33}` -> `{0.8, -2.7, 4.5, -5, 8.33, 8.5, 14.5}`

---

## Slides 102--103 — Loop Techniques: Selection Sort

Idea: At position i in [0, n-2], find the smallest element from i to n-1, then swap with a[i].

```c
void absSelectionSort(double a[], int n){
  for(int i = 0; i < n - 1; i++){
    int idxMin = i;
    for(int j = i + 1; j < n; j++){
      if(fabs(a[idxMin]) > fabs(a[j]))
        idxMin = j;
    }
    swap(a[idxMin], a[i]);
  }
}
```

---

## Slide 104 — Loop Techniques in Matrix: typedef

For small matrices, use `typedef`:

```c
#define MaxSz 20
typedef double MATRIX[MaxSz][MaxSz];

void mtPrint(MATRIX& A, int n){
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++)
      printf("%lf ", A[i][j]);
    printf("\n");
  }
}

void main(){
  MATRIX A = {{1, 2, 3}, {0, 4, 1}, {1, 5, 6}};
  mtPrint(A, 3);
}
```

---

## Slides 105--106 — Matrix Scanning Methods

**Method 1**: Scan each row (top to bottom), at each row scan all columns (left to right):
```c
for(int i = 0; i < m; i++)
  for(int j = 0; j < n; j++)
    // process a[i][j]
```

**Method 2**: Scan each column (left to right), at each column scan all rows:
```c
for(int j = 0; j < n; j++)
  for(int i = 0; i < m; i++)
    // process a[i][j]
```

**Scan fixed-k row**: `for(int j = 0; j < n; j++) // process a[k][j]`

**Scan fixed-k column**: `for(int i = 0; i < m; i++) // process a[i][k]`

---

## Slide 107 — Matrix: Sum at Row

```c
typedef double MAT[10][10];
double sumR(MAT& A, int n, int k){
  if(k < 0 || k >= n || n <= 0 || n > 10)
    return 0;
  double sum = 0;
  while(n--){
    sum+=A[k][n];
  }
  return sum;
}
```

---

## Slide 108 — Matrix: Diagonal Scanning

**Main diagonal**: `a[i][i]`
```c
for(int i = 0; i < n; i++)
  // process a[i][i];
```

**Sub diagonal**: `a[i][n-1-i]`
```c
for(int i = 0; i < n; i++)
  // process a[i][n - 1 - i];
```

---

## Slides 109--110 — Matrix: Above/Below Diagonal

**Above main diagonal** (j > i):
```c
for(int i = 0; i < n; i++)
  for(int j = i + 1; j < n; j++)
    // process a[i][j];
```

**Below main diagonal** (j < i):
```c
for(int i = 0; i < n; i++)
  for(int j = 0; j < i; j++)
    // process a[i][j];
```

**Above sub diagonal** (j < n-1-i):
```c
for(int i = 0; i < n; i++)
  for(int j = 0; j < n - 1 - i; j++)
    // process a[i][j];
```

**Below sub diagonal** (j >= n-i):
```c
for(int i = 0; i < n; i++)
  for(int j = n - i; j < n; j++)
    // process a[i][j];
```

---

## Slides 111--113 — Matrix Example 1: Check Row Sums Ascending

Check if sum of all elements is ascending from top to bottom.

```c
typedef double MATRIX[3][3];

double sumR(MATRIX &A, int n, int r){
  double s = 0;
  while(n--) s+=A[r][n];
  return s;
}

bool isIncrease(MATRIX &A, int n){
  bool result = true;
  int i = n - 1;
  double s2 = sumR(A, n, i);
  while(i--){
    double s1 = sumR(A, n, i);
    if(s1 > s2) result = false;
    s2 = s1; // Save s1 to compare with above row
  }
  return result;
}
```

---

## Slides 114--115 — Matrix Example 2: Sort Rows by Descending Sum

Sort square matrix such that sum of row elements is descending from top to bottom (selection sort on rows):

```c
void swapR(MATRIX &A, int n, int i, int k){
  for(int j = 0; j < n; j++)
    swap(A[i][j], A[k][j]);
}

bool sortR(MATRIX &A, int n){
  for(int i = 0; i < n - 1; i++){
    int idxMax = i;
    double smax = sumR(A, n, i);
    for(int j = i + 1; j < n; j++){
      double sj = sumR(A, n, j);
      if(sj > smax) {
        idxMax = j; smax = sj;
      }
    }
    swapR(A, n, i, idxMax);
  }
}
```

---

## Slides 116--117 — Matrix Example 3: Duplicate Rows

Check if square matrix contains coincided (duplicate) rows:

```c
bool rowsEqual(MATRIX &A, int n, int i, int k){
  while(n--)
    if(A[i][n] != A[k][n])
      return false;
  return true;
}

bool rowsDup(MATRIX &A, int n){
  for(int i = 0; i < n - 1; i++)
    for(int k = i + 1; k < n; k++)
      if(rowsEqual(A, n, i, k))
        return true;
  return false;
}
```

---

## Slide 118 — Matrix Example 4: Smallest Positive on Diagonal

Find the smallest positive number at main diagonal:

```c
int PositiveMinOnDiag(MATRIX& A, int n){
  int idx = -1;
  for(int i = 0; i < n; i++){
    if(A[i][i] > 0)
      if(idx == -1 || A[idx][idx] > A[i][i])
        idx = i;
  }
  return idx;
}
```

---

## Slide 119 — Matrix Example 5: Search Below Sub Diagonal

Check if there exists x below the sub diagonal:

```c
bool TriangleSearch(MATRIX& A, int n, double x){
  for(int i = 0; i < n; i++){
    for(int j = n - i; j < n; j++)
      if(A[i][j] == x)
        return true;
  }
  return false;
}
```

---

## Slide 120 — Recursion: Overview

Recursion algorithm includes:
- Linear recursion
- Binary recursion
- Non-linear recursion
- Mutual recursion

---

## Slide 121 — Recursion Example 1: Factorial

n! = (n-1)! * n, with 0! = 1

```c
long Factorization(long n){
  if(n <= 0) return 1;
  long kq = n*Factorization(n - 1);
  return kq;
}
void main(){
  long r;
  r = Factorization(5);
  cout << r << endl;
}
```

> Call trace: F(5) -> F(4) -> F(3) -> F(2) -> F(1) -> F(0)=1 -> 1 -> 2 -> 6 -> 24 -> 120

---

## Slide 122 — Recursion Example 2: Sum of Array

S = a[0] + ... + a[n-1], where S_n = S_(n-1) + a[n-1], S_0 = 0

```c
long sum(long a[], int n){
  if(n <= 0) return 0;
  long kq = a[n - 1] + sum(a, n - 1);
  return kq;
}
void main(){
  long b[]={1,2,3,4,5}, r;
  r = sum(b, 5);
  cout << r << endl;
}
```

> Call trace: sum(b,5) -> ... -> sum(b,0)=0 -> 1 -> 3 -> 6 -> 10 -> 15

---

## Slide 123 — Recursion Example 3: Euler Function

E_n = 1/1^k + 1/2^k + ... + 1/n^k, where E_n = E_(n-1) + 1/n^k

```c
long eulerFunc(int n, int k){
  if(n <= 0 || k < 1) return 0;
  int a = 1/pow(n, k);
  int b = eulerFunc(n - 1, k);
  return a + b;
}
```

---

## Slide 124 — Recursion Example 4: Max Element

If array size is one, it is the only max value. Else find max among first n-1 elements, compare with a[n-1].

```c
double Max(double a[], int n){
  if(n <= 1) return a[0];
  double m = Max(a, n - 1);
  if(m < a[n - 1]) m = a[n - 1];
  return m;
}
```

---

## Slide 125 — Recursion Example 5: sin & cos

```c
#define epsilon 0.000001

double Sin(double x){
  if(-epsilon<=x && x <= epsilon)
    return x - x*x*x/6;
  return 2*Sin(x/2)*Cos(x/2);
}

double Cos(double x){
  if(-epsilon<=x && x <= epsilon)
    return 1 - x*x/2;
  return 1 - 2*Sin(x/2)*Sin(x/2);
}
```

---

## Slide 126 — Recursion Example 5: sin/cos Trace

> *[Visual: Computation tree for Sin and Cos showing recursive halving of x until |x| < epsilon, then reconstructing values]*

---

## Slide 127 — Recursion Example 6: Square Root Approximation

x0 = 1 if n = 0; xn = x(n-1)/2 + a/(2*x(n-1)) for n >= 1, a in R*

```c
double Sqrt(int n, double a){
  double t;
  if(a <= 0) return 0;
  if(n <= 0) return 1;
  t = Sqrt(n - 1, a);
  return t/2 + a/(2*t);
}
```

Trace for Sqrt(6, 5): 1 -> 3 -> 7/3 -> 47/21 -> 2207/987 -> 2.236067978 -> 2.236067978

---

## Slide 128 — Recursion Example 7: Quick Exponentiation

- x^n = (x^(n/2))^2 if n is even
- x^n = x * (x^((n-1)/2))^2 if n is odd

```c
double Power(double x, int n){
  double r;
  if(n < 0) return 1/(Power(x, -n));
  if(n == 0) return 1;
  if(n == 1) return x;
  r = Power(x, n/2);
  if(n % 2 == 0) return r*r;
  return r*r*x;
}
```

Trace: Power(2, 5) -> Power(2, 2) -> Power(2, 1) = 2, r*r = 4, r*r*2 = 32
