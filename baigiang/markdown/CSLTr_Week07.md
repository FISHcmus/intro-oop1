# Week 07: Pointer (P2)

> **Source**: CSLTr_week07.ppsx (132 slides)
> **Advisor**: Truong Toan Thinh
> **Note**: Extracted from PPSX XML. Images extracted to `week07_images/`. Diagrams are composed from individual icons — described in text where possible.

---

## Slide 1 — Title

POINTER (P2)
Fundamentals of programming – Co so lap trinh
Advisor: Truong Toan Thinh

---

## Slide 2 — Contents

- Introduction
- Fixed-row 2D array
- Fixed-col 2D array
- Dynamic 2D array
- Dynamic multi-dimensional array
- Structure data
- Structural memory
- Operator [] for structural variable
- Structural array
- Enhanced techniques

---

## Slide 3 — Introduction

- 2D array includes `m` rows and `n` columns
- May use pointer or `vector<T>` to build 2D array
- Pointer-views of array:
  - 1D array: one pointer points to memory
  - 2D array: array of pointers pointing to memory
  - 3D array: 2D array of pointers pointing to memory
  - ...
- Need overloading operator `[]` for convenience

---

## Slide 4 — Introduction (Memory Diagrams)

> *[Visual content: Memory diagrams showing 1D, 2D, and 3D pointer layouts with addresses — refer to `week07_images/`]*

1D: `p` -> `<100>` (single pointer to memory block)
2D: `p` -> `<500>`, `<1000>` (array of pointers, each pointing to memory)
3D: `p` -> nested pointer arrays

---

## Slide 5 — Fixed-Row 2D Array

- Be a matrix with dynamic columns
- Need to provide a number of columns used
- Need to destroy memory after using
- Need to declare 1D array of pointers standing for a **fixed-number** of rows
- Note some syntaxes of accessing the matrix's elements
- Need to have a method of allocation preventing the memory's fragmentation

---

## Slide 6 — Fixed-Row 2D Array (Code)

```cpp
int arr2D_alloc(int* b[], int m, int n){
  int i, Success = 1;
  for(i = 0; i < m; i++){
    b[i] = (int*)malloc(n*sizeof(int));
    if(b[i] == NULL) Success = 0;
  }
  return Success;
}

void main(){
  int n, *a[4];
  cin >> n;
  arr2D_alloc(a, 4, n);
}
```

Memory access notes:
- `&a` = `(a+0)` = `<10>`
- `*(a+0)` = `a[0]` = `<100>`
- `*(*(a+0))` = `a[0][0]` = `1`

---

## Slide 7 — Fixed-Row 2D Array (Improved Allocation)

Need to improve `arr2D_alloc`: immediately exit when failed to allocate. Add `int* a[4] = {NULL}` into `main()`.

```cpp
int arr2D_alloc(int* a[], int m, int n){
  int i, Success = 1;
  for(i = 0; i < m; i++){
    a[i] = (int*)malloc(n*sizeof(int));
    if(a[i] == NULL) {
      Success = 0; break;
    }
  }
  return Success;
}
```

---

## Slide 8 — Fixed-Row 2D Array (Prevent Fragmentation)

Need to improve `arr2D_alloc`: prevent the fragmentation when using malloc.

```cpp
int arr2D_alloc(int* a[], int m, int n){
  int* buf = (int*)calloc(m*n, sizeof(int));
  if(buf == NULL){
    a[0] = NULL; return 0;
  }
  a[0] = buf;
  for(int i = 1; i < m; i++){
    buf += n; a[i] = buf;
  }
  return 1;
}
```

> *[Visual content: Memory layout showing contiguous allocation with `buf` pointer and `a` array of row pointers]*

---

## Slide 9 — Fixed-Row 2D Array (Template)

Using template in C++ to generalize datatype:

```cpp
template <class T>
int arr2D_alloc(T* a[], int m, int n){
  T* buf = (T*)calloc(m*n, sizeof(T));
  if(buf == NULL){
    a[0] = NULL; return 0;
  }
  a[0] = buf;
  for(int i = 1; i < m; i++){
    buf += n; a[i] = buf;
  }
  return 1;
}
```

---

## Slide 10 — Fixed-Col 2D Array

- Be a matrix with dynamic rows
- Need to provide a number of rows used
- Need to destroy memory after allocation
- Consider this is an array of 1D static arrays
- Note some syntaxes of accessing matrix's elements

---

## Slide 11 — Fixed-Col 2D Array (Code)

```cpp
typedef float floatArr1D[3]; // float[3] <-> floatArr1D

floatArr1D* arr2D_alloc(int m){
  floatArr1D* a; // float[3]* a
  a = (floatArr1D*)calloc(m, sizeof(floatArr1D));
  return a;
}

void arr2D_input(floatArr1D* a, int m, int n){
  for(int i = 0; i < m; i++)
    for(int j = 0; j < n; j++)
      cin >> a[i][j];
}
```

Memory access notes:
- `&a` = `<50>`; `a+0` = `a[0]` = `<100>`; `a+1` = `a[1]` = `<112>`
- `*(a+0)` = `<100>`; `*(a+1)` = `<112>`
- `*(*(a+1) + 2)` = `a[1][2]` = `6`

---

## Slide 12 — Fixed-Col 2D Array (Using struct with template)

```cpp
#define NCOL 4
template <class T>
struct array1D{
  T data[NCOL + 1];
  T& operator[](int i){  // accessing without '.'
    if(i >= 0 && i < NCOL) return data[i];
    return data[NCOL];
  }
};

// Example:
void main(){
  array1D<int> a; a[0] = 2;  // a.data[0] = 2
  cout << a[0] << endl;
}
```

---

## Slide 13 — Fixed-Col 2D Array (Template Functions)

```cpp
template <class T>
void arr2D_alloc(array1D<T>* &a, int m){
  a = (array1D<T>*)calloc(m, sizeof(array1D<T>));
}

template <class T>
void arr2D_input(array1D<T>* a, int m, int n){
  for(int i = 0; i < m; i++)
    for(int j = 0; j < n; j++)
      cin >> a[i][j];
}

template <class T>
void arr2D_free(array1D<T>* a){
  if(a != NULL) free(a);
}

void main(){
  int mB, nB = NCOL;
  array1D<float>* B;
  cin >> mB;  // Input row
  arr2D_alloc(B, mB);
  arr2D_input(B, mB, nB);
  arr2D_free(B);
}
```

---

## Slide 14 — Dynamic 2D Array

- Be a matrix with dynamic rows and columns
- Need to provide a number of columns and rows
- Need to destroy memory after allocation
- There are many methods of constructing:
  1. Allocate m row pointers, and each pointer points to address of memories of n elements
  2. Allocate m row pointers, and a common memory of m x n elements
  3. Allocate a common memory of m pointers and m x n elements

---

## Slide 15 — Dynamic 2D Array (table_alloc v1)

Using `int arr2D_alloc(T* a[], int m, int n)` at "fixed row" to build a function of creating dynamic matrix:

```cpp
template <class T>
int arr2D_alloc(T* a[], int m, int n){
  T* buf = (T*)calloc(m*n, sizeof(T));
  if(buf == NULL){ a[0] = NULL; return 0; }
  a[0] = buf;
  for(int i = 1; i < m; i++){
    buf += n; a[i] = buf;
  }
  return 1;
}

float** table_alloc(int m, int n){  // ex: m = n = 3
  float** a = (float**)calloc(m, sizeof(float*));
  if(a == NULL) return NULL;
  if(arr2D_alloc(a, m, n) == 0){ free(a); a = NULL; }
  return a;
}
```

---

## Slide 16 — Dynamic 2D Array (table_alloc v2)

```cpp
// version 1 (returns pointer)
float** table_alloc(int m, int n){...}

// version 2 (output parameter)
void table_alloc(float*** a, int m, int n){
  *a = table_alloc(m, n);
}

void main(){
  float** b;
  table_alloc(&b, 3, 3);
}
```

---

## Slide 17 — Dynamic 2D Array (table_free)

Using `void arr2D_free(T* a[])` at "fixed row" to build a function of destroying dynamic matrix:

```cpp
template <class T>
void arr2D_free(T* a[]){
  if(a[0] != NULL){
    free(a[0]); a[0] = NULL;
  }
}

void table_free(float** a, int m){  // ex: m = 3
  if(a == NULL || m <= 0) return;
  arr2D_free(a);
  free(a);
}
```

---

## Slide 18 — Dynamic 2D Array (Usage)

Guiding of using `table_alloc` and `table_free`:

```cpp
void main(){
  int row, col;
  cin >> row >> col;  // ex: row = col = 3
  float** B = table_alloc(row, col);
  table_free(B, row);
}
```

---

## Slide 19 — Dynamic 2D Array (Template table_alloc/free)

Add `template` to `table_alloc` and `table_free`. Note: `arr2D_alloc` and `arr2D_free` are template functions.

```cpp
template <class T>
void table_alloc(T*** a, int m, int n){
  *a = (T**)calloc(m, sizeof(T*));
  if(*a == NULL) return;
  if(!arr2D_alloc(*a, m, n)){
    free(*a); *a = NULL;
  }
}

template <class T>
void table_free(T** a, int m){
  if(a == NULL || m <= 0) return;
  arr2D_free(a);
  free(a);
}
```

---

## Slide 20 — Dynamic 2D Array (Method 1)

Method 1: allocate m row pointers, each pointer points to memory's address of n elements.

```cpp
void arr2D_alloc(float*** a, int m, int n){
  int Success = 1;
  *a = (float**)calloc(m, sizeof(float*));
  for(int i = 0; i < m; i++){
    (*a)[i] = (float*)calloc(n, sizeof(float));
    if((*a)[i] == NULL){
      Success = 0; break;
    }
  }
  if(!Success){
    arr2D_free(*a, m); *a = NULL;
  }
}
```

---

## Slide 21 — Dynamic 2D Array (Method 1 — Free)

```cpp
void arr2D_free(float** a, int m){
  if(a == NULL || m <= 0) return;
  for(int i = 0; i < m; i++){
    if(a[i] != NULL) free(a[i]);
  }
  free(a);
}

void main(){
  int d, c; float** B;
  cin >> d >> c;  // d=4; c=3
  arr2D_alloc(&B, d, c); arr2D_input(B, d, c);
  arr2D_output(B, d, c); arr2D_free(B, d);
}
```

---

## Slide 22 — Dynamic 2D Array (Method 1 — Template C++)

```cpp
template <class T>
void arr2D_alloc(T*** a, int m, int n){
  int Success = 1;
  *a = (T**)calloc(m, sizeof(T*));
  for(int i = 0; i < m; i++){
    (*a)[i] = (T*)calloc(n, sizeof(T));
    if((*a)[i] == NULL){ Success = 0; break; }
  }
  if(!Success){ arr2D_free(*a, m); *a = NULL; }
}

template <class T>
void arr2D_free(T** a, int m){
  if(a == NULL || m <= 0) return;
  for(int i = 0; i < m; i++){
    if(a[i] != NULL) free(a[i]);
  }
  free(a);
}
```

---

## Slide 23 — Dynamic 2D Array (Method 1 — void* in C)

```c
void arr2D_alloc(void*** a, int m, int n, int szItem){
  int Success = 1;
  *a = (void**)calloc(m, sizeof(void*));
  for(int i = 0; i < m; i++){
    (*a)[i] = (void*)calloc(n, szItem);
    if((*a)[i] == NULL){ Success = 0; break; }
  }
  if(!Success){ arr2D_free(*a, m); *a = NULL; }
}

void arr2D_free(void** a, int m){
  if(a == NULL || m <= 0) return;
  for(int i = 0; i < m; i++){
    if(a[i] != NULL) free(a[i]);
  }
  free(a);
}
```

---

## Slide 24 — Dynamic 2D Array (Method 2)

Method 2: Allocate m row pointers, and a common memory of m x n elements.

```cpp
void arr2D_alloc(float*** a, int m, int n){
  if(m <= 0 || n <= 0) return;
  *a = (float**)calloc(m, sizeof(float*));
  if(*a == NULL) return;
  float* buf = (float*)calloc(m * n, sizeof(float));
  if(!buf) { free(*a); return; }
  (*a)[0] = buf;  // *(*a + 0) = buf
  for(int i = 1; i < m; i++){
    buf += n; (*a)[i] = buf;  // *(*a + i) = buf
  }
}
```

---

## Slide 25 — Dynamic 2D Array (Method 2 — void* in C)

```c
void arr2D_alloc(void*** a, int m, int n, int sItem){
  if(m <= 0 || n <= 0) return;
  *a = (void**)calloc(m, sizeof(void*));
  if(*a == NULL) return;
  void* buf = (void*)calloc(m * n, szItem);
  if(!buf) { free(*a); return; }
  *((*a) + 0) = buf; int szRow = n * szItem;
  for(int i = 1; i < m; i++){
    buf = (char*)buf + szRow;
    *(*a + i) = buf;
  }
}
```

---

## Slide 26 — Dynamic 2D Array (Method 2 — Template C++)

```cpp
template <class T>
void arr2D_alloc(T*** a, int m, int n){
  if(m <= 0 || n <= 0) return;
  *a = (T**)calloc(m, sizeof(T*));
  if(*a == NULL) return;
  T* buf = (T*)calloc(m * n, sizeof(T));
  if(!buf) { free(*a); return; }
  (*a)[0] = buf;
  for(int i = 1; i < m; i++){
    buf += n; (*a)[i] = buf;
  }
}

template <class T>
void arr2D_free(T** a){
  if(!a){
    if(a[0] != NULL) free(a[0]);
    free(a);
  }
}
```

---

## Slide 27 — Dynamic 2D Array (Method 2 — Usage)

Demonstrate how to use `void*` and `template`:

```cpp
void main(){
  int d, c;
  float** B;
  cin >> d >> c;
  arr2D_alloc((void***)&B, d, c, sizeof(float));
  // arr2D_alloc<float>(&B, d, c);
  arr2D_input(B, d, c);
  arr2D_output(B, d, c);
  arr2D_free((void**)B);
  // arr2D_free<float>(B);
}
```

---

## Slide 28 — Dynamic 2D Array (Method 3)

Method 3: Allocate a common memory of m row pointers and m x n elements.

```cpp
void arr2D_alloc(float*** a, int m, int n){
  if(m <= 0 || n <= 0) return;
  int sz1 = m*sizeof(float*);  // example m = 4
  int sz2 = m*n*sizeof(float); // example n = 3
  void* buf = calloc(sz1 + sz2, 1);
  if(buf == NULL) return;
  *a = (float**)buf; buf = (char*)buf + sz1;
  (*a)[0] = (float*)buf;
  for(int i = 1; i < m; i++){
    buf = (float*)buf + n; (*a)[i] = (float*)buf;
  }
}
```

---

## Slide 29 — Dynamic 2D Array (Method 3 — void* in C)

```c
void arr2D_alloc(void*** a, int m, int n, int szItem){
  if(m <= 0 || n <= 0) return;
  int sz1 = m * sizeof(void*);
  int sz2 = m * n * szItem, szRow = n * szItem;
  void* buf = calloc(sz1 + sz2, 1);
  if(buf == NULL) return;
  *a = (void**)buf; buf = (char*)buf + sz1;
  (*a)[0] = (void*)buf;
  for(int i = 1; i < m; i++){
    buf = (char*)buf + szRow;
    (*a)[i] = buf;
  }
}
```

---

## Slide 30 — Dynamic 2D Array (Method 3 — Template C++)

```cpp
template <class T>
void arr2D_alloc(T*** a, int m, int n){
  if(m <= 0 || n <= 0) return;
  int sz1 = m * sizeof(T*);
  int sz2 = m * n * sizeof(T);
  void* buf = calloc(sz1 + sz2, 1);
  if(buf == NULL) return;
  *a = (T**)buf; buf = (char*)buf + sz1;
  (*a)[0] = (T*)buf;
  for(int i = 1; i < m; i++){
    buf = (T*)buf + n;
    (*a)[i] = (T*)buf;
  }
}
```

---

## Slide 31 — Dynamic 2D Array (Method 3 — Free)

Method 3: implement destroy function very easily.

```cpp
void arr2D_free(float** a){
  if(a != NULL) free(a);
}

void arr2D_free(void** a){
  if(a != NULL) free(a);
}

template <class T>
void arr2D_free(T** a){
  if(a != NULL) free(a);
}
```

---

## Slide 32 — Dynamic 2D Array (Convenient Methods — Goals)

Goals:
- Simple and easy-to-use programming interface
- Can be used with various datatype
- Should build the functions with simple datatypes
- Process the memory problems with pointer
- Lightweight source code

Some key functions:
- `void alloc2D(void***, int, int, int);`
- `void free2D(void**);`
- `void resize2D(void***, int, int);`
- `int nRow(void**);`
- `int nCol(void**);`

---

## Slide 33 — Dynamic 2D Array (Convenient Methods — Data Structure)

Data structure (Example of an array of 2 rows, 3 columns with `float`):

| Field | Description |
|-------|-------------|
| Rows (m) | 2 |
| Columns (n) | 3 |
| Size of datatype | 4 |
| `<30>` | address of the first element at row 01 |
| `<42>` | address of the first elements at row 02 |

---

## Slide 34 — Dynamic 2D Array (Convenient Methods — alloc2D)

How to use from `main()`:

```cpp
void main(){
  int d, c, **B; cin >> d >> c;  // d = 2, c = 3
  alloc2D((void***)&B, d, c, sizeof(int));
}

void alloc2D(void*** a, int m, int n, int szItem){
  if(m <= 0 || n <= 0 || szItem <= 0) return;
  int szRow = n*szItem, sz1 = m*sizeof(void*), sz2 = m*szRow;
  void* buf = calloc(12 + sz1 + sz2, 1);  // 12 = headsize
  if(buf == NULL) return;
  int* bs = (int*)buf;
  bs[0] = m; bs[1] = n; bs[2] = szItem;
  buf = (char*)buf + 12;
  *a = (void**)buf;
  buf = (char*)buf + sz1; (*a)[0] = buf;
  for(int i = 1; i < m; i++){
    buf = (char*)buf + szRow;
    (*a)[i] = buf;
  }
}
```

---

## Slide 35 — Dynamic 2D Array (Convenient Methods — nRow/nCol)

```cpp
void main(){
  int d, c, **B; cin >> d >> c;  // d = 2, c = 3
  alloc2D((void***)&B, d, c, sizeof(int));
  arr2D_Input(B);
}

void arr2D_Input(int* a[]){
  int m = nRow((void**)a), n = nCol((void**)a);
  for(int i = 0; i < m; i++)
    for(int j = 0; j < n; j++)
      cin >> a[i][j];
}

int nRow(void** c){
  int* bs = (int*)((char*)c - 12);
  return bs[0];
}

int nCol(void** d){
  int* bs = (int*)((char*)d - 12);
  return bs[1];
}
```

---

## Slide 36 — Dynamic 2D Array (Convenient Methods — resize2D)

```cpp
void main(){
  //...
  arr2D_Output(B);  // easy
  resize2D((void***)&B, 3, 4);
}

void resize2D(void*** a, int m, int n){  // m = 3, n = 4
  if(*a == NULL || m <= 0 || n <= 0) return;
  int* bs = (int*)((char*)(*a) - 12);
  int m_old = bs[0], n_old = bs[1], szItem_old = bs[2];
  void** anew; alloc2D(&anew, m, n, szItem_old);
  if(anew == NULL) return;
  for(int i = 0; i < m_old && i < m; i++){
    for(int j = 0; j < n_old && j < n; j++){
      char* d = (char*)anew[i] + j * szItem_old;
      char* s = (char*)(*a)[i] + j * szItem_old;
      memmove(d, s, szItem_old);
    }
  }
  free2D(*a); *a = anew;
}
```

---

## Slide 37 — Dynamic 2D Array (Convenient Methods — free2D)

```cpp
void main(){
  //...
  arr2D_Output(B);  // easy
  resize2D((void***)&B, 3, 4);
  free2D((void**)B);
}

void free2D(void** a){
  if(a != NULL){
    void* p = (char*)a - 12;
    free(p);
  }
}
```

---

## Slide 38 — Dynamic 2D Array (Convenient Methods — Template C++)

```cpp
template <class T>
void arr2D_alloc(T*** a, int m, int n){
  if(m <= 0 || n <= 0) return;
  int szRow=n*sizeof(T), sz1=m*sizeof(T*), sz2=m*szRow;
  void* buf = calloc(8 + sz1 + sz2, 1);
  if(buf == NULL) return;
  int* bs = (int*)buf;
  bs[0] = m; bs[1] = n;
  buf = (char*)buf + 8;
  *a = (T**)buf;
  buf = (char*)buf + sz1; (*a)[0] = (T*)buf;
  for(int i = 1; i < m; i++){
    buf = (char*)buf + szRow;
    (*a)[i] = (T*)buf;
  }
}
```

Note: Template version uses headsize = 8 (no `szItem` field needed). The `void*` version uses headsize = 12 (includes `szItem`).

---

## Slide 39 — Dynamic 2D Array (Convenient Methods — Template nRow/nCol/free2D)

```cpp
template <class T>
int nRow(T** c){
  int* bs = (int*)((char*)c - 8);
  return bs[0];
}

template <class T>
int nCol(T** d){
  int* bs = (int*)((char*)d - 8);
  return bs[1];
}

template <class T>
void free2D(T** a){
  if(a != NULL){
    void* p = (char*)a - 8; free(p);
  }
}

// void* versions (headsize = 12):
int nRow(void** c){
  int* bs = (int*)((char*)c - 12);
  return bs[0];
}
int nCol(void** d){
  int* bs = (int*)((char*)d - 12);
  return bs[1];
}
void free2D(void** a){
  if(a != NULL){
    void* p = (char*)a - 12; free(p);
  }
}
```

---

## Slide 40 — Dynamic 2D Array (Convenient Methods — Template resize2D)

```cpp
template <class T>
void resize2D(T*** a, int m, int n){
  if(*a == NULL || m <= 0 || n <= 0) return;
  int* bs = (int*)((char*)(*a) - 8);
  int m_old = bs[0], n_old = bs[1];
  T** anew; alloc2D(&anew, m, n);
  if(anew == NULL) return;
  for(int i = 0; i < m_old && i < m; i++){
    for(int j = 0; j < n_old && j < n; j++){
      anew[i][j] = (*a)[i][j];  // Overload '=' if T is not basic
    }
  }
  free2D(*a); *a = anew;
}

// void* version:
void resize2D(void*** a, int m, int n){
  if(*a == NULL || m <= 0 || n <= 0) return;
  int* bs = (int*)((char*)(*a) - 12);
  int m_old = bs[0], n_old = bs[1], szItem_old = bs[2];
  void** anew; alloc2D(&anew, m, n, szItem_old);
  if(anew == NULL) return;
  for(int i = 0; i < m_old && i < m; i++){
    for(int j = 0; j < n_old && j < n; j++){
      char* d = (char*)anew[i] + j * szItem_old;
      char* s = (char*)(*a)[i] + j * szItem_old;
      memmove(d, s, szItem_old);
    }
  }
  free2D(*a); *a = anew;
}
```

---

## Slide 41 — Dynamic Multi-Dimensional Array (3D Array)

- Rarely used as with 1D/2D arrays
- Processing can be made based from operations of 1D or 2D array
- Still can build pure convenient functions for 3D array
- 3D array has a number of rows, columns and high/depth (m x n x r)
- There are two methods of building convenient functions based on 1D/2D array:
  - Similar to 2D array (m x n), and each element is a 1D array having r elements
  - Similar to 1D array having m elements, and each element is a 2D array (n x r)

---

## Slide 42 — 3D Array (Two Approaches Diagram)

> *[Visual content: Two diagrams showing 3D array layouts for m=2, n=3, r=2]*

- Approach 1: 2D array (m x n) where each element is a 1D array of r elements
- Approach 2: 1D array of m elements where each element is a matrix (n x r)

---

## Slide 43 — 3D Array (First Solution — Helper Functions)

The first solution: ex m = r = 2 and n = 3.

Remind some functions:
- `int arrSize(void* aData)`
- `void arrFree(void* aData)`
- `void* arrInit(int n, int sz)`

---

## Slide 44 — 3D Array (First Solution — 2D Helper Functions)

Remind some functions:
- `void alloc2D(void*** a, int m, int n, int sz)`
- `void free2D(void** a)`
- `int nRow(void** a)`
- `int nCol(void** a)`

---

## Slide 45 — 3D Array (First Solution — alloc3D)

```cpp
float*** alloc3D(int m, int n, int r){
  if(m <= 0 || n <= 0 || r <= 0) return NULL;
  float*** b; int err = 0;
  alloc2D((void***)(&b), m, n, sizeof(float*));
  if(b == NULL) return NULL;
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      b[i][j] = (float*)arrInit(r, sizeof(float));
      if(b[i][j] == NULL) { err = 1; }
    }
  }
  if(err == 1){
    free3D(b);
    b = NULL;
  }
  return b;
}
```

---

## Slide 46 — 3D Array (First Solution — free3D)

```cpp
void free3D(float*** a){
  if(a != NULL){
    int m = nRow((void**)a);
    int n = nCol((void**)a);
    for(int i = 0; i < m; i++){
      for(int j = 0; j < n; j++){
        arrFree(a[i][j]);
      }
    }
    free2D((void**)a);
  }
}
```

---

## Slide 47 — 3D Array (First Solution — arr3D_input)

```cpp
void arr3D_input(float*** a){
  int m = nRow((void**)a);
  int n = nCol((void**)a);
  int r = arrSize(a[0][0]);
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++){
      for(int k = 0; k < r; k++){
        cin >> a[i][j][k];
        // cin >> *(*(*(a+i)+j)+k);
      }
    }
  }
}
```

---

## Slide 48 — 3D Array (First Solution — main)

```cpp
void main(){
  float*** a = alloc3D(2, 3, 2);
  arr3D_input(a);
  arr3D_output(a);
  free3D(a);
}
```

---

## Slide 49 — 3D Array (Second Solution — alloc3D)

The second solution: 1D array with m elements being a matrix n x r.

```cpp
float*** alloc3D(int m, int n, int r){
  if(m <= 0 || n <= 0 || r <= 0) return NULL;
  float*** a = (float***)arrInit(m, sizeof(float**));
  if(a == NULL) return NULL;
  int error = 0;
  for(int i = 0; i < m; i++){
    alloc2D((void***)&a[i], n, r, sizeof(float));
    if(a[i] == NULL){ error = 1; break; }
  }
  if(error == 1){ free3D(a); a = NULL; }
  return a;
}
```

---

## Slide 50 — 3D Array (Second Solution — free3D)

```cpp
void free3D(float*** a){
  if(a != NULL){
    int m = arrSize((void*)a);
    for(int i = 0; i < m; i++){
      if(a[i] != NULL){
        free2D((void**)a[i]);
      }
    }
    arrFree((void*)a);
  }
}
```

---

## Slide 51 — 3D Array (Second Solution — arr3D_input)

```cpp
void arr3D_input(float*** a){
  int m = arrSize((void*)a);
  int n = nRow((void**)a[0]);
  int r = nCol((void**)a[0]);
  for(int i = 0; i < m; i++){
    for(int j = 0; j < n; j++)
      for(int k = 0; k < r; k++)
        cin >> a[i][j][k];
  }
}
```

---

## Slide 52 — 3D Array (Second Solution — main)

```cpp
void main(){
  float*** a = alloc3D(2, 3, 2);
  arr3D_input(a);
  arr3D_output(a);
  free3D(a);
}
```

---

## Slide 53 — Free Memory (Importance)

What if source code has any memory-error:
- Inefficiently using memory
- Easy to cause interruption when running
- RAM may run out if any program using this source code runs again and again
- More serious if any memory-error program is running in service provider (server)
- Data which are not freed may be sensitive, such as password or financial key...
- The bytes being freed are assigned the value of `0xDD`

---

## Slide 54 — Structural Data

- In practice, data follow another structure
- Use keyword `struct` combined with `typedef` to define a new datatype
- Use operator `.` or `->` to access to all members of a structure
- Use operator `sizeof` combined with `pragma pack(1)` to exactly determine the size of a new datatype
- Use keyword `union` if wanting some members of a structure to share a common memory

---

## Slide 55 — Structural Data (Size)

Example (need using `pragma pack(1)`):

```cpp
typedef struct {
  long offset;
  union{ unsigned long cw; unsigned char cb[4]; };
  char mb;
} HeadStruct;

typedef struct { long x, y; } Point;
typedef struct { short nVers; Point Vers[1]; } Polygon;
typedef struct { short nVers; Point* Vers; } PolygonP;
```

```cpp
void main(){
  HeadStruct aDat;
  HeadStruct* pDat = &aDat;
}
```

| Expression | Value |
|---|---|
| `sizeof(aDat)` | 9 |
| `sizeof(pDat)` | 4 |
| `sizeof(HeadStruct)` | 9 |
| `sizeof(Point)` | 8 |
| `sizeof(Polygon)` | 10 |
| `sizeof(PolygonP)` | 6 |

---

## Slide 56 — Structural Data (Member Address)

Using the same struct declarations with `pragma pack(1)`:

```cpp
void main(){
  HeadStruct aDat;
  HeadStruct* pDat = &aDat;
}
```

| Expression | Address |
|---|---|
| `&aDat` | `61ff17` |
| `&aDat.offset` | `61ff17` |
| `&aDat.cw` | `61ff1b` |
| `&aDat.cb` | `61ff1b` |
| `&aDat.mb` | `61ff1f` |

---

## Slide 57 — Structural Data (Pointer Member Address)

```cpp
void main(){
  HeadStruct aDat;
  HeadStruct* pDat = &aDat;
}
```

| Expression | Address |
|---|---|
| `&pDat` | `61ff10` |
| `pDat` | `61ff17` |
| `&(pDat->offset)` | `61ff17` |
| `pDat->cb` | `61ff1b` |
| `&(pDat->cw)` | `61ff1b` |
| `&(pDat->mb)` | `61ff1f` |

Note: `pDat->cb` = `&(pDat->cb)`

---

## Slide 58 — Structural Data (Union Value)

```cpp
void main(){
  HeadStruct aDat;
  HeadStruct* pDat = &aDat;
  (*pDat).cw = 0x44332211;
}
```

| Expression | Value |
|---|---|
| `*(pDat->cb+0)` | `11` |
| `pDat->cb[1]` | `22` |
| `*(pDat->cb+2)` | `33` |
| `pDat->cb[3]` | `44` |
| `pDat->cw` | `44332211` |
| `*((unsigned long*)((char*)pDat+4))` | `44332211` |

---

## Slide 59 — Structural Data (Initialization Technique)

```cpp
HeadStruct h = {1};
// h.offset = 1, all other fields = 0

PolygonP pg = {0};
// pg.nVers = 0, pg.Vers = NULL

Polygon pg = {1, {2, 3}};
// pg.nVers = 1, pg.Vers[0].x = 2, pg.Vers[0].y = 3
```

---

## Slide 60 — Structural Data (PolygonP — Method 1 Alloc)

```cpp
typedef struct { long x, y; } Point;
typedef struct { short nVers; Point* Vers; } PolygonP;

PolygonP* PgAllocA(int n){
  if(n < 0) return NULL;
  int szHead = sizeof(short) + sizeof(Point*);
  PolygonP* pg = (PolygonP*)calloc(szHead, 1);
  if(pg == NULL) return NULL;
  pg->nVers = n;
  pg->Vers = (Point*)calloc(n, sizeof(Point));
  if(pg->Vers == NULL){ free(pg); pg = NULL; }
  return pg;
}

void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  PolygonP* A = PgAllocA(4);
}
```

---

## Slide 61 — Structural Data (PolygonP — Method 1 Assign)

```cpp
void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  PolygonP* A = PgAllocA(4);
  if(A != NULL){
    for(int i = 0; i < 4; i++){
      A->Vers[i] = P[i];
    }
  }
}
```

---

## Slide 62 — Structural Data (PolygonP — Method 1 Free)

```cpp
void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  PolygonP* A = PgAllocA(4);
  if(A != NULL){
    for(int i = 0; i < 4; i++) A->Vers[i] = P[i];
    pgFreeA(A);
  }
}

void pgFreeA(PolygonP* pg){
  if(pg != NULL){
    if(pg->Vers != NULL) free(pg->Vers);
    free(pg);
  }
}
```

---

## Slide 63 — Structural Data (PolygonP — Method 2 Alloc)

```cpp
PolygonP* PgAllocB(int n){
  if(n < 0) return NULL;
  int szHead = sizeof(short) + sizeof(Point*);
  int szData = n * sizeof(Point);
  void* buf = calloc(szHead + szData, 1);
  if(buf == NULL) return NULL;
  PolygonP* pg = (PolygonP*)buf;
  pg->nVers = n;
  pg->Vers = (Point*)((char*)buf + szHead);
  return pg;
}

void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  PolygonP* B = PgAllocB(4);
}
```

---

## Slide 64 — Structural Data (PolygonP — Method 2 Assign)

```cpp
void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  PolygonP* B = PgAllocB(4);
  if(B != NULL){
    for(int i = 0; i < 4; i++){
      B->Vers[i] = P[i];
    }
  }
}
```

---

## Slide 65 — Structural Data (PolygonP — Method 2 Free)

```cpp
void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  PolygonP* B = PgAllocB(4);
  if(B != NULL){
    for(int i = 0; i < 4; i++) B->Vers[i] = P[i];
    pgFreeB(B);
  }
}

void pgFreeB(PolygonP* pg){
  if(pg != NULL) free(pg);
}
```

---

## Slide 66 — Structural Data (Polygon — One-Member Technique)

```cpp
typedef struct { long x, y; } Point;
typedef struct { short nVers; Point Vers[1]; } Polygon;

Polygon* PgAlloc(int n){
  if(n < 0) return NULL;
  int szHead = sizeof(Polygon), szData = (n-1)*sizeof(Point);
  Polygon* pg = (Polygon*)calloc(szHead + szData, 1);
  if(pg == NULL) return NULL;
  pg->nVers = n;
  return pg;
}

void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  Polygon* A = PgAlloc(4);
}
```

---

## Slide 67 — Structural Data (Polygon — Assign)

```cpp
void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  Polygon* A = PgAlloc(4);
  if(A != NULL){
    for(int i = 0; i < A->nVers; i++)
      A->Vers[i] = P[i];
  }
}
```

---

## Slide 68 — Structural Data (Polygon — Free)

```cpp
void main(){
  Point P[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  Polygon* A = PgAlloc(4);
  if(A != NULL){
    for(int i = 0; i < A->nVers; i++)
      A->Vers[i] = P[i];
    pgFree(A);
  }
}

void pgFree(Polygon* pg){
  if(pg != NULL) free(pg);
}
```

---

## Slide 69 — Structural Memory

- Use structural variable to replace the computation on memory address
- Help source-code be abstract, easy-to-read and easy-to-maintain
- Program is more efficient due to without computation on memory address
- Apply structural variables for the functions of 1D/2D

---

## Slide 70 — Structural Memory (1D Array — Structure)

Review example of 1D array:

```cpp
typedef struct {
  int nItem, sizeItem;
  char Data[1];
} ArrayStruct;

static int headSize = sizeof(int) + sizeof(int);
```

> *[Visual content: Memory layout showing `nItem`, `sizeItem`, and `Data` fields]*

---

## Slide 71 — Structural Memory (1D Array — StructOf)

```cpp
ArrayStruct* StructOf(void* aData){
  if(aData != NULL) aData = (char*)aData - headSize;
  return (ArrayStruct*)aData;
}
```

---

## Slide 72 — Structural Memory (1D Array — arrSize)

```cpp
int arrSize(void* aData){
  ArrayStruct* as = StructOf(aData);
  if(as != NULL) return as->nItem;
  return 0;
}
```

---

## Slide 73 — Structural Memory (1D Array — arrItemSize)

```cpp
int arrItemSize(void* aData){
  ArrayStruct* as = StructOf(aData);
  if(as != NULL) return as->sizeItem;
  return 0;
}
```

---

## Slide 74 — Structural Memory (1D Array — arrFree)

```cpp
void arrFree(void* aData){
  ArrayStruct* as = StructOf(aData);
  if(as != NULL) free(as);
}
```

---

## Slide 75 — Structural Memory (1D Array — arrInit)

```cpp
static int memSize(int nItem, int sizeItem){
  if(sizeItem < 0) sizeItem = -sizeItem;
  if(sizeItem == 0) sizeItem = 1;
  if(nItem < 0) nItem = -nItem;
  return headSize + nItem * sizeItem;
}

void* arrInit(int n, int szItem){  // ex: n = 3 & szItem = 4
  int sz = memSize(n, szItem);  // Compute the bytes needed
  void* aOrigin = malloc(sz), *aData = NULL;
  if(aOrigin != NULL){
    ArrayStruct* as = (ArrayStruct*)aOrigin;
    memset(aOrigin, 0, sz);
    as->nItem = n;      // Assign value of nItem into nItem member
    as->sizeItem = szItem;  // Assign value of sizeItem into sizeItem member
    aData = as->Data;
  }
  return aData;
}
```

---

## Slide 76 — Structural Memory (1D Array — arrResize)

```cpp
void* arrResize(void* aData, int n){  // n=2
  if(aData == NULL || n < 0) return NULL;
  ArrayStruct* as = StructOf(aData);
  int szItem = as->sizeItem;
  int szNew = memSize(n, szItem);
  ArrayStruct* aNew = (ArrayStruct*)realloc(as, szNew);
  if(aNew != NULL){
    aNew->nItem = n;
    return aNew->Data;
  }
  return NULL;
}
```

---

## Slide 77 — Structural Memory (1D Array — Usage)

```cpp
void main(){
  ArrayStruct* B = (ArrayStruct*)arrInit(2, sizeof(int));
  if(B != NULL){
    *((int*)(B->Data) + 0) = 1;
    *((int*)(B->Data) + 1) = 2;
    *((int*)(B->Data) + 2) = 3;
    cout << *((int*)(B->Data) + 0) << ", "
         << *((int*)(B->Data) + 1) << ", "
         << *((int*)(B->Data) + 2) << endl;
    arrFree(B);
  }
}
```

---

## Slide 78 — Structural Memory (2D Array — Structure)

Review example of 2D array:

```cpp
typedef struct {
  int m, n, sizeItem;
  void* Data[1];
} aStruct;

static int headSize = sizeof(int) + sizeof(int) + sizeof(int);
```

---

## Slide 79 — Structural Memory (2D Array — nRow)

```cpp
int nRow(void** aData){
  aStruct* as = (aStruct*)((char*)aData - headSize);
  if(as != NULL) return as->m;
  return 0;
}
```

---

## Slide 80 — Structural Memory (2D Array — nCol)

```cpp
int nCol(void** aData){
  aStruct* as = (aStruct*)((char*)aData - headSize);
  if(as != NULL) return as->n;
  return 0;
}
```

---

## Slide 81 — Structural Memory (2D Array — Free2D)

```cpp
void Free2D(void** aData){
  if(aData != NULL){
    void* p = (char*)aData - headSize;
    free(p);
  }
}
```

---

## Slide 82 — Structural Memory (2D Array — alloc2D)

```cpp
void alloc2D(void*** a, int m, int n, int sz){  // m=2, n=3, sz=4
  if(m <= 0 || n <= 0 || sz <= 0) return;
  int szRow = n * sz;       // bytes per row
  int sz1 = m * sizeof(void*);  // bytes of pointers' array1D
  int sz2 = m * szRow;      // bytes of all data
  void* buf = calloc(headSize + sz1 + sz2, 1);
  if(buf == NULL) return;
  aStruct* as = (aStruct*)buf;
  as->m = m; as->n = n; as->sizeItem = sz;
  buf = (char*)buf + headSize + sz1;
  as->Data[0] = buf;
  for(int i = 1; i < m; i++){
    buf = (char*)buf + szRow;
    as->Data[i] = buf;
  }
  *a = as->Data;
}
```

---

## Slide 83 — Structural Memory (2D Array — Usage)

```cpp
void main(){
  int** A;
  alloc2D((void***)&A, 2, 3, sizeof(int));
  arr2D_Input(A);
  arr2D_Output(A);
  free2D((void**)A);
}
```

---

## Slide 84 — Structural Memory (Float Standard 32-bit IEEE)

Example of float 32-bit:

| Byte 0 (bits 0-7) | Byte 1 (bits 8-15) | Byte 2 (bits 16-23) | Byte 3 (bits 24-31) |
|---|---|---|---|
| Bits Mantissa | Bits Mantissa | Bits Expo | Bit Sign + Bits Expo |

```cpp
#pragma GCC diagnostic ignored "-Wpedantic"
typedef union{
  float Value;
  unsigned long dWord;
  unsigned short Words[2];
  unsigned char Bytes[4];
  struct{
    unsigned long Mantissa: 23;
    unsigned int Expo: 8;
    unsigned int Sign: 1;
  };
} floatStruct;
```

---

## Slide 85 — Structural Memory (Float — floatDump)

```cpp
void floatDump(floatStruct* fs){
  if(fs == NULL) return;
  printf("-------------------------\n");
  printf(" +value: %f\n", fs->Value);
  printf(" +stored dword: 0x%lX\n", fs->dWord);
  printf(" +stored words: 0x%04X 0x%04X\n", fs->Words[0], fs->Words[1]);
  printf(" +stored bytes: ");
  for(int i = 0; i < sizeof(float); i++)
    printf("0x%02X  ", fs->Bytes[i]);
  printf("\n");
  printf(" + IEEE stored parts:\n");
  printf("   Sign: %d\n", fs->Sign);
  printf("   Expo: 0x%X\n", fs->Expo);
  printf("   Mantissa: 0x%lX\n", fs->Mantissa);
}
```

---

## Slide 86 — Structural Memory (Float — Usage)

```cpp
void main(){
  float x = 1/(float)3;
  unsigned char bytes[] = {0x89, 0x01, 0x46, 0xC1};  // bytes = -12.375375f
  floatStruct* p = (floatStruct*)&x;
  floatDump(p);
  p = (floatStruct*)bytes;
  floatDump(p);
}
```

---

## Slide 87 — Structural Memory (Float — Output)

```
-------------------------
+ value: 0.333333
+ stored dword: 0x3EAAAAAB
+ stored words: 0xAAAB  0x3EAA
+ stored bytes: 0xAB  0xAA  0xAA  0x3E
+ IEEE stored parts:
   Sign: 0
   Expo: 0x7D
   Mantissa: 0x2AAAAB
-------------------------
+ value: -12.375375
+ stored dword: 0xC1460189
+ stored words: 0x0189  0xC146
+ stored bytes: 0x89  0x01  0x46  0xC1
+ IEEE stored parts:
   Sign: 1
   Expo: 0x82
   Mantissa: 0x460189
```

---

## Slide 88 — Structural Memory (16-bit Text File — Structure)

```cpp
typedef struct{
  union{
    unsigned char markBytes[2];
    unsigned short markWord;
  };
  wchar_t Data[1];
} unicode16_Text;
```

Example file "Xin chao Viet Nam":
`0xFF 0xFE 0x58 0x69 0x6e 0x20 0x63 0x68 0xe0 0x6f 0x20 0x56 0x69 0x1ec7 0x74 0x20 0x4e 0x61 0x6d`

---

## Slide 89 — Structural Memory (16-bit Text File — readFileUtf16)

```cpp
unicode16_Text* readFileUtf16(char* f, long* m){
  unicode16_Text* p = NULL;
  FILE* fp = fopen(f, "rb");
  if(fp != NULL){
    fseek(fp, 0, SEEK_END);
    *m = ftell(fp);
    p = (unicode16_Text*)calloc(*m, 1);
    if(p != NULL){
      fseek(fp, 0, SEEK_SET);
      fread(p, *m, 1, fp);
    }
    fclose(fp);
  }
  return p;
}
```

---

## Slide 90 — Structural Memory (16-bit Text File — Usage)

```cpp
void main(){
  long fsize;
  unicode16_Text* p = readFileUtf16("abc.inp", &fsize);
  long nChar = (fsize - 2)/2;
  if(p != NULL) cout << nChar << endl;
  for(int i = 0; i < nChar; i++)
    cout << hex << "0x" << (short)p->Data[i] << "|";
  free(p);
}
```

---

## Slide 91 — Operator [] for Structural Variable

- This operator allows to further define with complex datatypes
- Use this operator helps to save memory reserved for pointers (review 2D/3D array)
- Prototype of this operator must return a reference
- This operator should be defined in `struct` block

---

## Slide 92 — Operator [] (Round Array)

Example of round array: define operator `[]` to freely transmit negative/positive/out-of-bound index. Example: `a[-1]` or `a[MAX + 1]`...

```cpp
template <class T>
struct roundArray{
  int nItem;
  T* arr, Dummy;
  // Need '&' because operator '[]' is left-exp of operator assignment '='
  T& operator[](int i){
    if(arr == NULL || nItem <= 0) return Dummy;
    if(i >= nItem) i = i % nItem;
    else if(i < 0) {
      int j = (-i) % nItem;
      if(j == 0) i = 0;
      else i = nItem - j;
    }
    return arr[i];
  }
};
```

---

## Slide 93 — Operator [] (Round Array — Init/Free)

```cpp
template <class T>
void roundArrayInit(roundArray<T>& a, int n){
  a.nItem = 0; a.arr = NULL;
  if(n <= 0) return;
  a.nItem = n; a.arr = new T[n];
}

template <class T>
void roundArrayFree(roundArray<T>& a){
  if(a.arr != NULL){
    delete[] a.arr; a.arr = NULL;
  }
}
```

---

## Slide 94 — Operator [] (Round Array — Usage)

```cpp
void main(){
  roundArray<int> A; int n = 5;
  roundArrayInit<int>(A, n);
  for(int i = 0; i < A.nItem; i++) A[i] = i;
  for(int i = -7; i <= 17; i++){
    cout << "  " << A[i];
    if(i % n == 0) cout << endl;
  }
  roundArrayFree(A);
}
```

---

## Slide 95 — Operator [] (2D Array with void*)

```cpp
struct array2D{
  int nRow, nCol, szItem; void* buf;
  void* operator[](int i){
    if(i < 0 || i >= nRow) i = 0;
    return (char*)buf + i * nCol * szItem;
  }
};

bool arr2D_Init(array2D& a, int m, int n, int sz){
  if(m <= 0 || n <= 0 || sz <= 0) return false;
  a.nRow = m; a.nCol = n; a.szItem = sz;
  a.buf = calloc(m * n, sz);
  if(a.buf == NULL) return false;
  return true;
}

void arr2D_free(array2D& a){
  if(a.buf != NULL) free(a.buf);
}

void main(){
  array2D A;
  if(arr2D_Init(A, 2, 3, sizeof(int))){
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 3; j++)
        cin >> ((int*)A[i])[j];  // Example of inputing 1 2 3 4 5 6
    arr2D_free(A);
  }
}
```

---

## Slide 96 — Operator [] (2D Array — Template Comparison)

Side-by-side comparison of `void*` vs `template` approach:

| void* version | template version |
|---|---|
| `struct array2D{` | `template <class T> struct array2D{` |
| `int nRow, nCol, szItem; void* buf;` | `int nRow, nCol; T* buf;` |
| `void* operator[](int i){` | `T* operator[](int i){` |
| `return (char*)buf + i*nCol*szItem;` | `return buf + i*nCol;` |
| `bool arr2D_Init(array2D& a, int m, int n, int sz)` | `template<class T> bool arr2D_Init(array2D<T>& a, int m, int n)` |
| `a.buf = calloc(m*n, sz);` | `a.buf = (T*)calloc(m*n, sizeof(T));` |
| `cin >> ((int*)A[i])[j];` | `cin >> A[i][j];` |

---

## Slide 97 — Operator [] (2D Array — Template Usage)

```cpp
template <class T>
void arr2D_free(array2D<T>& a){
  if(a.buf != NULL) free(a.buf);
}

void main(){
  array2D<int> A;
  if(arr2D_Init<int>(A, 2, 3)){
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 3; j++)
        cin >> A[i][j];  // input 1 2 3 4 5 6
    arr2D_free<int>(A);
  }
}
```

---

## Slide 98 — Operator [] (3D Array)

Example of 3D array: define operator `[]` in 3D structure:

```cpp
template <class T>
struct array3D{
  int nRow, nCol, nHigh; T* buf;
  array2D<T> operator[](int i){
    if(i < 0 || i >= nRow) i = 0;
    array2D<T> a2D;
    a2D.nRow = nCol; a2D.nCol = nHigh;
    a2D.buf = buf + i * nCol * nHigh;
    return a2D;
  }
};

template <class T>
bool arr3D_Init(array3D<T>& a, int m, int n, int r){
  if(m <= 0 || n <= 0 || r <= 0) return false;
  a.nRow = m; a.nCol = n; a.nHigh = r;
  a.buf = (T*)calloc(m * n * r, sizeof(T));
  if(a.buf == NULL) return false;
  return true;
}

template <class T>
void arr3D_free(array3D<T>& a){
  if(a.buf != NULL) free(a.buf);
}

void main(){
  array3D<int> A;
  if(arr3D_Init<int>(A, 2, 3, 2)){
    for(int i = 0; i < 2; i++)
      for(int j = 0; j < 3; j++)
        for(int k = 0; k < 2; k++)
          cin >> A[i][j][k];  // input 1 2 3 4 5 6 7 8 9 10 11 12
    arr3D_free<int>(A);
  }
}
```

---

## Slide 99 — Operator [] (3D Array — Improved with Caching)

```cpp
template <class T>
struct array3D{
  int nRow, nCol, nHigh, iCur; T* buf;
  array2D<T> a2DCur;
  array2D<T> operator[](int i){
    if(i < 0 || i >= nRow) i = 0;
    if(i != iCur){
      a2DCur.nRow = nCol; a2DCur.nCol = nHigh;
      a2DCur.buf = buf + i * nCol * nHigh;
      iCur = i;
    }
    return a2DCur;
  }
};

template <class T>
bool arr3D_Init(array3D<T>& a, int m, int n, int r){
  if(m <= 0 || n <= 0 || r <= 0) return false;
  a.nRow = m; a.nCol = n; a.nHigh = r;
  a.iCur = -1; a.a2DCur = {0};
  a.buf = (T*)calloc(m * n * r, sizeof(T));
  if(a.buf == NULL) return false;
  return true;
}
```

---

## Slide 100 — Structural Array

- Be an array, in which each element has a user-defined datatype (using `struct`)
- New datatype may have fixed or dynamic members
- With fixed member, we can directly allocate an amount of bytes (May use the method of creating 1D array as previous examples)
- With dynamic member, we need to specially process

---

## Slide 101 — Structural Array (Student Structure)

```cpp
typedef struct{
  string Code, FamilyName, Name;
  char BirthDate[11];
  float Grade1, Grade2, Grade3, GPA;
} Student;

void main(){
  Student** lst = stInit(0), st;
  string stcode = "nocode";
  while(1){
    cin >> stcode;
    if(stcode == "---") break;
    st.Code = stcode;
    getStudent(st);
    if(StPush(&lst, st) == 0) break;
  }
  int n = arrSize((void*)lst);
  for(int i = 0; i < n; i++){
    // cout << lst[i]->Code ...
  }
  StFree(lst);
}
```

---

## Slide 102 — Structural Array (Student — StInit)

```cpp
Student** StInit(int n){
  Student** a = NULL;
  if(n < 0) n = 0;
  a = (Student**)arrInit(n, sizeof(Student*));
  return a;
}
```

---

## Slide 103 — Structural Array (Student — getStudent)

```cpp
void getStudent(Student& sv){
  cin.ignore();
  char str[256]; float g1, g2, g3;
  cin.getline(str, 256); sv.FamilyName = str;
  cin.getline(str, 256); sv.Name = str;
  cin.getline(sv.BirthDate, 256);
  cin >> g1 >> g2 >> g3;
  sv.GPA = (g1 + g2 + g3)/3;
  sv.Grade1 = g1; sv.Grade2 = g2; sv.Grade3 = g3;
}
```

---

## Slide 104 — Structural Array (Student — StPush)

```cpp
int StPush(Student*** a, const Student& sv){
  Student* t = new Student; *t = sv;
  return arrPushback((void**)a, (void*)&t);
}

int arrPushback(void** aData, void* x){
  int nItem = arrSize(*aData), szItem = arrItemSize(*aData);
  void* aNew = arrResize(*aData, 1 + nItem);
  if(aNew != NULL){
    memmove((char*)aNew + nItem * szItem, x, szItem);
    *aData = aNew;
    return 1;
  }
  return 0;
}
```

---

## Slide 105 — Structural Array (Student — StFree)

```cpp
void StFree(Student** a){
  if(a == NULL) return;
  int n = arrSize(a);
  for(int i = 0; i < n; i++){
    if(a[i] != NULL) delete a[i];
  }
  arrFree(a);
}

void arrFree(void* aData){
  if(aData != NULL) free(origin_addr(aData));
}
```

---

## Slide 106 — Structural Array (Polygon Array — File Structure)

Build structural array of polygons. Use read/write binary files. Reuse the functions of 1D Array.

Structure of file read/written:

```
<vertex n1><x1><y1>...<x_n1><y_n1>
<vertex n2><x1><y1>...<x_n2><y_n2>
...
<vertex nm><x1><y2>...<x_nm><y_nm>
```

(m polygons)

---

## Slide 107 — Structural Array (Polygon — Init & Push)

```cpp
typedef struct{ int x, y; } Point;
typedef struct{ short nVer; Point* Vers; } Polygon;

void main(){
  Point dg1[] = {{0, 0}, {0, 5}, {5, 5}}; int n1 = 3;
  Point dg2[] = {{0, 0}, {0, 5}, {5, 5}, {5, 0}}; int n2 = 4;
  Point dg3[] = {{0, 0}, {0, 5}, {1, 1}, {5, 5}, {5, 0}}; int n3 = 5;
  Polygon* lst = PolyListInit();
  if(lst != NULL){
    PolyListPush(&lst, dg1, n1);
    PolyListPush(&lst, dg2, n2);
    PolyListPush(&lst, dg3, n3);
    PolyListSave(lst, "Polygons.dat");
    PolyListFree(lst);
  }
}

Polygon* PolyListInit(){
  void* a = arrInit(0, sizeof(Polygon));
  return (Polygon*)a;
}
```

---

## Slide 108 — Structural Array (Polygon — PolyListPush)

```cpp
int PolyListPush(Polygon** t, Point* P, short n){
  Polygon po = {n};
  po.Vers = (Point*)calloc(n, sizeof(Point));
  for(int i = 0; i < n; i++) po.Vers[i] = P[i];
  return arrPushback((void**)t, (void*)&po);
}
```

---

## Slide 109 — Structural Array (Polygon — After Second Push)

> *[Visual content: Memory layout after pushing dg1 (3 vertices) and dg2 (4 vertices) — `lst` array has 2 Polygon elements]*

---

## Slide 110 — Structural Array (Polygon — After Third Push)

> *[Visual content: Memory layout after pushing all three polygons — `lst` array has 3 Polygon elements with vertex counts 3, 4, 5]*

---

## Slide 111 — Structural Array (Polygon — PolyListSave)

```cpp
void PolyListSave(Polygon* t, const char* fname){
  if(t == NULL || fname == NULL) return;
  FILE* fp = fopen(fname, "wb");
  if(fp == NULL) return;
  int nPoly = arrSize(t);
  for(int i = 0; i < nPoly; i++){
    short n = t[i].nVer;
    void* pData = t[i].Vers;
    fwrite(&n, sizeof(n), 1, fp);
    fwrite(pData, sizeof(Point), n, fp);
  }
  fclose(fp);
}
```

---

## Slide 112 — Structural Array (Polygon — PolyListFree)

```cpp
void PolyListFree(Polygon* t){
  if(t == NULL) return;
  int n = arrSize(t);
  for(int i = 0; i < n; i++)
    if(t[i].Vers != NULL) free(t[i].Vers);
  arrFree(t);
}
```

---

## Slide 113 — Structural Array (Re-reading Binary File)

Build a structural array of polygons to reread binary file just created. There are 2 ways:
1. Create new `struct` with one-member technique
2. Reuse `Polygon*`

---

## Slide 114 — Structural Array (Method 1 — PolygonList Structure)

```cpp
typedef struct{ short nVer; Point Vers[1]; } PolygonDat;
typedef struct{
  char* buf;     // Data
  long nBytes;   // Size of file
  int nPoly;     // Amount of polygons
  PolygonDat* operator[](int i){ /* ... */ }
} PolygonList;

void main(){
  // portion of creating file as previous example
  PolygonList pList;
  InitPolyList(pList, "Polygons.dat");
  if(pList.buf != NULL){
    for(int i = 0; i < pList.nPoly; i++){
      PolygonDat* pg = pList[i];
      ShowPoly(pg); cout << endl;
    }
    free(pList.buf);
  }
}

void InitPolyList(PolygonList& p, const char* fname){
  p.nPoly = 0;
  p.buf = PolyRead(fname, p.nBytes);
  CountPoly(p);
}
```

---

## Slide 115 — Structural Array (Method 1 — PolyRead)

```cpp
char* PolyRead(const char* fname, long& size){
  char* b = NULL;
  FILE* fp = fopen(fname, "rb");
  if(fp != NULL){
    fseek(fp, 0, SEEK_END); size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    b = (char*)malloc(size);
    if(b != NULL) fread(b, size, 1, fp);
    fclose(fp);
  }
  return b;
}
```

---

## Slide 116 — Structural Array (Method 1 — CountPoly)

```cpp
void CountPoly(PolygonList& pl){
  if(pl.buf == NULL) return;
  char* pStart = pl.buf, *pEnd = pl.buf + pl.nBytes;
  while(pStart < pEnd){
    short nVer = *(short*)pStart;
    if(nVer > 0) pl.nPoly++;
    pStart += sizeof(short) + nVer * sizeof(Point);
  }
}
```

---

## Slide 117 — Structural Array (Method 1 — operator[])

```cpp
typedef struct{
  char* buf;
  long nBytes;
  int nPoly;
  PolygonDat* operator[](int i){
    int j = 0; char* p = buf;
    while(j < i){
      short nVer = *(short*)p;
      p += sizeof(short) + nVer * sizeof(Point);
      j++;
    }
    return (PolygonDat*)p;
  }
} PolygonList;
```

---

## Slide 118 — Structural Array (Method 1 — ShowPoly)

```cpp
void ShowPoly(PolygonDat* pg){
  cout << pg->nVer << " dinh: ";
  for(int i = 0; i < pg->nVer; i++)
    cout << "(" << pg->Vers[i].x << ", " << pg->Vers[i].y << ") ";
}
```

---

## Slide 119 — Structural Array (Method 2 — PolyRead with Polygon*)

Reuse `struct` Point & Polygon:

```cpp
typedef struct{ int x, y; } Point;
typedef struct{ short nVer; Point* Vers; } Polygon;

Polygon* PolyRead(const char* fname, int& n){
  FILE* fp = fopen(fname, "rb");
  if(fp == NULL) return NULL;
  n = 0;
  Polygon* pList = (Polygon*)arrInit(0, sizeof(Polygon));
  while(!feof(fp)){
    Polygon g = {0};
    if(fread(&(g.nVer), sizeof(g.nVer), 1, fp) <= 0) break;
    g.Vers = (Point*)malloc(g.nVer * sizeof(Point));
    if(g.Vers != NULL){
      n++;
      fread(g.Vers, g.nVer, sizeof(Point), fp);
      arrPushback((void**)&pList, (void*)&g);
    }
  }
  fclose(fp);
  return pList;
}

void main(){
  // portion of creating file as previous example
  int nPoly;
  Polygon* lst = PolyRead("Polygons.dat", nPoly);
  if(lst){
    for(int i = 0; i < nPoly; i++){
      ShowPoly(&lst[i]); cout << endl;
    }
  }
  PolyListFree(lst);
}
```

---

## Slide 120 — Structural Array (Method 2 — ShowPoly)

```cpp
void ShowPoly(Polygon* pg){
  cout << pg->nVer << " vertex: ";
  for(int i = 0; i < pg->nVer; i++)
    cout << "(" << pg->Vers[i].x << ", " << pg->Vers[i].y << ") ";
}
```

---

## Slide 121 — Structural Array (Method 2 — PolyListFree)

```cpp
void PolyListFree(Polygon* t){
  if(t == NULL) return;
  int n = arrSize(t);
  for(int i = 0; i < n; i++)
    if(t[i].Vers != NULL) free(t[i].Vers);
  arrFree(t);
}
```

---

## Slide 122 — Enhanced Techniques (void* Pointer)

- Be a pointer with datatype of `void`, for example `void* p`
- Use in case of unknown datatype in practice
- Disadvantage of this pointer is that we cannot compute with memory address. For example: cannot code `p + 1` or `*(p - 1)` because p with `void*` cannot 'jump'
- May use template in C++ to replace

---

## Slide 123 — Enhanced Techniques (void* Pointer — Example)

```cpp
void main(){
  int a[] = {1, 2, 3, 4, 5};
  void* p = a;
  cout << p[1] << endl;           // Wrong
  cout << *((int*)p + 1) << endl; // OK, prints 2
  cout << ((int*)p)[1] << endl;   // OK, prints 2
}
```

---

## Slide 124 — Enhanced Techniques (Reference)

A variable referring to another one. Reference variable needs original variable, for it cannot exist independently.

There are 3 ways to use reference:
1. Declare reference variable referring (with same datatype) to original one in the same scope. Example: `int a = 5; int& b = a;`
2. Be a reference parameter of another function. Example: `Func(<datatype>& x)`
3. Returned value of another function is a reference variable. Example: `<datatype>& Func()`. Note: original variable must exist when returning a reference (not refer to local variable in function)

May use reference variable for pointer.

---

## Slide 125 — Enhanced Techniques (Reference — Example)

```cpp
void main(){
  int a = 5;
  int& b = a;
  a = 6;
  cout << "Value of a: " << a << endl;      // 6
  cout << "Value of b: " << b << endl;      // 6
  cout << "Address of a: " << &a << endl;   // same address
  cout << "Address of b: " << &b << endl;   // same address
}
```

---

## Slide 126 — Enhanced Techniques (Data & Const Pointer)

Pointer can "const" the memory of data:

```cpp
void main(){
  char a[] = "Hello world!!!";
  const char* pstr;
  a[0] = 'h';          // Right
  pstr = a;
  pstr[0] = 'H';       // Wrong
  a[0] = 'H';          // Right
  pstr = "a another string";  // Right (pstr can point to another address)
}
```

Notes:
- `pstr = (char*)pstr` runs OK but nonsense because data cannot be changed with this pointer
- Can cast datatype but must return another different pointer. Example: `char* tmp = (char*)pstr;` — then we can use `tmp` to edit string

---

## Slide 127 — Enhanced Techniques (Const Pointer)

**Const pointer** is a pointer only pointing to **one** address when it is initialized and defined:

```cpp
void main(){
  char a[] = "Hello world!!!";
  char* const pstr = a;  // Declare and define simultaneously
  pstr[0] = 'h';         // Right
  *(pstr + 1) = 'E';     // Right
  pstr = pstr + 1;       // Wrong
}
```

Can **change value** of const pointer by using type-casting combined with reference:

```cpp
void main(){
  char a[] = "Hello world!!!";
  char* const pstr = a;
  pstr[0] = 'h';         // Right
  *(pstr + 1) = 'E';     // Right
  char*& tmp = (char*&)pstr;
  tmp++;
  cout << pstr[0] << endl;  // print 'E'
}
```

Note: directly using `pstr` still produces error message. Example: `pstr++` and `pstr = (char*)pstr` are WRONG.

---

## Slide 128 — Enhanced Techniques (Const for Structural/Base Types)

We may use `const` for structural/base non-pointer datatype to "const" data.

```cpp
Point const P = {0, 1};     // or: const Point P = {0, 1};
int const a[] = {1, 2};     // or: const int a[] = {1, 2};
```

Notes:
- With both ways, `a` and `&P` respectively have `const int*` and `const Point*`
- Changing values of `P.x`, `P.y` and the elements of array `a` is illegal
- May use type-casting to return a different variable to **change value** of the variables

---

## Slide 129 — Enhanced Techniques (Static Array)

Example of static 1D array:

```cpp
int c[] = {1, 2, 3}, *p = c;
```

Notes:
- `p` and `c` point to one address
- The statement `c = new int` is wrong because `c` is **const pointer**
- `&p` != `p` = `c` = `&c`. It can be said that `c` is a special pointer with hidden address
- `sizeof(p)` = `sizeof(int*)` = 4, but `sizeof(c)` = 12

Passing parameter to function:

```cpp
void main(){
  int c[] = {1, 2, 3}, *p = c;
  funcA(c); funcB(c);
  funcA(p); funcB(p);
}
void funcA(int a[]) { /* ... */ }
void funcB(int* b)  { /* ... */ }
```

---

## Slide 130 — Enhanced Techniques (Left-Value & Right-Value)

**Left-value** is the left expressions of assignment `=`. Examples: single var, pointer var, structural var (access by `.` and `->`), array-element variable (access by `[]`), pointer var combined with operator `*`.

Returned value can play a role of left-value:
- Returned value is a reference
- Returned value is base/structural pointer

**Right-value** is the right expressions of assignment `=`. All left-value expressions can play the roles of right-value expressions. The reverse direction is not sure. For example `&x` is only a right-value expression.

---

## Slide 131 — Enhanced Techniques (Left-Value — Reference & Pointer Return)

```cpp
int& maxRef(int& tx, int& ty){
  if(tx > ty) return tx;
  return ty;
}

int* minPtr(int* px, int* py){
  if(*px > *py) return py;
  return px;
}

int x = 99, y = 88;

void main(){
  maxRef(x, y) = 9988;
  cout << x;  // x = 9988
  *(minPtr(&x, &y)) = 7766;
  cout << y;  // y = 7766
}
```

---

## Slide 132 — Enhanced Techniques (Left-Value — Structural Pointer Return)

```cpp
typedef struct { int x, y; } Point;
Point P = {16, 10}, Q = {-8, 25};

Point* PointMinX(Point* A, Point* B){
  if(A->x > B->x) return B;
  return A;
}

void main(){
  PointMinX(&P, &Q)->x /= 2;    // Q.x = -4
  *PointMinX(&P, &Q) = P;       // Value Q is equal to P's
}
```
