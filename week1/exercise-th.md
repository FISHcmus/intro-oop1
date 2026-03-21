# BT TH Tuần 01 — OOP 2025B W1-W5 (Codeforces)

Contest: pbthai.contest.codeforces.com, contest ID 678864
Problems: A, B, C

---

## A. hello
**Time limit:** 1s | **Memory:** 256 MB

Viết chương trình nhập vào tên của một người và chào người đó.

### Input
Gồm một xâu `s` chỉ chứa các chữ cái latin, là tên của một người.

### Output
Ghi ra `XiN chAo s, chuc ban Hoc Tot`, với `s` là tên của người đó. Lưu ý là trình chấm có phân biệt chữ hoa chữ thường.

### Examples

**Input:**
```
Thai
```
**Output:**
```
XiN chAo Thai, chuc ban Hoc Tot
```

**Input:**
```
jkAxLZiu
```
**Output:**
```
XiN chAo jkAxLZiu, chuc ban Hoc Tot
```

---

## B. hello oop
**Time limit:** 1s | **Memory:** 256 MB

Xây dựng một lớp phân số với tên `Fraction` chứa các hàm:

- Khởi tạo mặc định: `Fraction();`
- Khởi tạo phân số num/den: `Fraction(int num, int den);`
- Chuyển sang dạng xâu: `string to_string()` trả về xâu dạng `num/den`.

Các hàm trên cần để public. Sinh viên được phép viết thêm các hàm và biến khác nếu cần.

**Lưu ý:** Yêu cầu của bài là thiết kế lớp nên sinh viên không cần viết hàm `main`. Thay vào đó, cần thêm dòng `#include "main.h"` vào cuối bài làm để chấm điểm.

---

## C. INTEGER
**Time limit:** 1s | **Memory:** 256 MB

Xây dựng một lớp `Integer` trong C++ để biểu diễn một số nguyên. Lớp này cần bao gồm một thuộc tính:

- `int value`: Giá trị của số nguyên.

Lớp cần có các phương thức sau:

- `Integer()`: Hàm tạo mặc định, khởi tạo giá trị mặc định là 0.
- `Integer(int val)`: Hàm tạo có tham số.
- `int getValue()`: Getter để lấy giá trị.
- `void setValue(int val)`: Setter để cập nhật giá trị.

**Lưu ý:** Yêu cầu thiết kế lớp, không cần viết `main`. Thêm `#include "main.h"` vào cuối bài.
