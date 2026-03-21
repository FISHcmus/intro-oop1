# BT TH Tuần 02 — OOP 2025B W1-W5 (Codeforces)

Contest: pbthai.contest.codeforces.com, contest ID 678864
Problems: D, E, F

---

## D. SORT
**Time limit:** 1s | **Memory:** 256 MB

Sắp xếp một dãy các phần tử, kiểu dữ liệu có thể là: Số nguyên, ký tự, phân số, xâu ký tự.

### Input
- Dòng đầu chứa yêu cầu sắp xếp: `orderType T` với `orderType` là `"increase"` hoặc `"decrease"`; `T` là `int`, `char`, `fraction`, `string`.
- Dòng thứ hai chứa `n` là số lượng phần tử.
- `n` dòng tiếp theo, mỗi dòng chứa một phần tử. Với phân số, định dạng nhập vào `a/b` là `a b`.

### Output
Ghi `n` dòng là các phần tử sau khi sắp xếp, theo định dạng như đầu vào.

### Examples

**Input:**
```
increase int
4
2
5
3
2
```
**Output:**
```
2
2
3
5
```

**Input:**
```
decrease string
6
Dai
hoc
Khoa
Hoc
Tu
Nhien
```
**Output:**
```
hoc
Tu
Nhien
Khoa
Hoc
Dai
```

**Input:**
```
increase fraction
4
3 5
3 2
1 2
4 6
```
**Output:**
```
1 2
3 5
4 6
3 2
```

---

## E. POINT3D
**Time limit:** 1s | **Memory:** 256 MB

Xây dựng một lớp Điểm trong không gian 3D với tên `Point3D` chứa các hàm:

- `Point3D()`: Khởi tạo mặc định.
- `Point3D(double x, double y, double z)`: Khởi tạo điểm với tọa độ (x,y,z).
- `double distance(Point3D other)`: Tính khoảng cách từ điểm hiện tại đến điểm khác.
- `void move(double dx, double dy, double dz)`: Di chuyển điểm theo vector (dx,dy,dz).
- `void rotate()`: Xoay điểm 90 độ theo chiều dương quanh gốc toạ độ (giữ nguyên trục Oz, quay x và y).

**Lưu ý:** Yêu cầu thiết kế lớp, không cần viết `main`. Thêm `#include "main.h"` vào cuối bài.

---

## F. Fraction
**Time limit:** 1s | **Memory:** 256 MB

Xây dựng một lớp Phân số với tên `Fraction` chứa các hàm:

- `Fraction()`: Khởi tạo mặc định.
- `Fraction(int num, int den)`: Khởi tạo phân số num/den.
- `std::string to_string()`: Trả về xâu dạng `num/den`.
- `Fraction add(Fraction other)`: Cộng hai phân số, kết quả tối giản.
- `Fraction subtract(Fraction other)`: Trừ hai phân số, kết quả tối giản.
- `Fraction multiply(Fraction other)`: Nhân hai phân số, kết quả tối giản.
- `Fraction divide(Fraction other)`: Chia hai phân số, kết quả tối giản.
- `void simplify()`: Rút gọn phân số (mẫu số dương, UCLN = 1).
- `int getNumerator()`: Getter tử số.
- `int getDenominator()`: Getter mẫu số.
- `void setNumerator(int num)`: Setter tử số.
- `void setDenominator(int den)`: Setter mẫu số.

**Lưu ý:** Ngoại trừ `simplify()` và các hàm phép toán, các hàm khác không được rút gọn phân số. Không cần viết `main`. Thêm `#include "main.h"` vào cuối bài.
