# F. treasure2

- **Source:** https://pbthai.contest.codeforces.com/group/9fnqSlqFiq/contest/682387/problem/F
- **Time limit:** 1 second
- **Memory limit:** 256 MB
- **Input / output:** standard

## Đề bài

Alice và Bob đang tham gia một trò chơi tìm kho báu. Quản trò chuẩn bị một
mê cung có **1026 phòng**, mỗi phòng có một bóng đèn đang bật hoặc tắt.

- Mỗi phòng kề với **nhiều nhất 2 phòng khác**, và có thể di chuyển giữa
  tất cả các phòng với nhau (mê cung liên thông). Do đó mê cung là một
  **đường thẳng (path)** hoặc một **chu trình (cycle)**.
- Kho báu được chôn ở một phòng **kề với đúng 2 phòng khác** (tức là đỉnh
  trong nếu mê cung là path, hoặc bất kỳ đỉnh nào nếu là cycle).

Diễn biến:

1. Quản trò dẫn **Alice** vào phòng chứa kho báu. Alice được tự do di
   chuyển, viết số lên sàn, và đổi trạng thái một số bóng đèn.
2. Sau đó, quản trò đưa Alice ra ngoài và **xoá sạch mọi dấu vết trên
   sàn**, nhưng **không tác động đến trạng thái bóng đèn**.
3. Quản trò dẫn **Bob** vào một phòng bất kỳ. Bob cũng được tự do di
   chuyển, viết số lên sàn, đổi trạng thái bóng đèn.
4. Bob phải dừng lại ở **đúng phòng Alice đã xuất phát** (phòng kho báu)
   rồi đào kho báu lên.

**Ràng buộc quan trọng:** số lần Alice gọi `flip` không vượt quá **2**.

## Hành động khi ở trong một phòng

- **Viết số lên sàn:** hệ thống tự thực hiện. Bắt đầu từ 1 và tăng dần
  mỗi khi nhân vật bước vào một phòng **chưa có số**.
- **Quan sát:** hệ thống tự trả về một **vector thông tin** gồm 3 số
  nguyên:
  - Số thứ nhất: `0` nếu bóng đèn phòng hiện tại tắt, `1` nếu bật.
  - Hai số sau: số đang được viết trên sàn của các phòng kế bên, tương
    ứng với hai bức tường của phòng hiện tại.
    - `-1`: bức tường đó là **biên** của mê cung (không có cửa).
    - `0`: sàn phòng bên kia **chưa được viết số**.
    - số nguyên dương `x`: sàn phòng bên kia **đã được viết số `x`**.
  - **Trong mê cung nhân vật mất phương hướng hoàn toàn**, nên thứ tự
    hai bức tường trong vector thông tin là **tuỳ ý** và **có thể khác
    nhau giữa các lần quan sát cùng một phòng**.
- **Đổi trạng thái đèn:** gọi `flip()`.
- **Kết thúc hành trình:** `return` khỏi `solveAlice` / `solveBob`.
- **Di chuyển:** gọi `move(i)`.

## API thư viện `treasure2lib.h`

```cpp
vector<int> move(int i);
```

- `i ∈ {1, 2}`: chọn bức tường thứ `i` theo **vector thông tin hiện
  tại** rồi đi qua cửa đó.
- Trả về vector thông tin của phòng mới bước vào.

```cpp
void flip();
```

- Đổi trạng thái bóng đèn của phòng nhân vật đang đứng.

## Hai hàm thí sinh cần cài

```cpp
void solveAlice(vector<int> s);
```

- `s` là vector thông tin của phòng mà Alice xuất phát (phòng kho báu).

```cpp
void solveBob(vector<int> s);
```

- `s` là vector thông tin của phòng mà Bob xuất phát.

## Lưu ý

- Trình chấm gọi `solveAlice` trước, sau đó gọi `solveBob` trên **một
  luồng khác với tài nguyên độc lập**. Hai hàm không chia sẻ biến toàn
  cục tại thời điểm chạy — mọi thông tin Alice muốn truyền cho Bob chỉ
  có thể đi qua **trạng thái bóng đèn**.
- Các hàm có thể được gọi **nhiều lần** (tối đa 5000 testcase). Thí sinh
  phải **tự khởi tạo lại biến toàn cục** giữa các lần gọi nếu cần.
- File bài làm phải tên là `treasure2.cpp`.
- Đầu file phải có `#include "treasure2lib.h"`.
- Được phép khai báo thêm thư viện, hàm phụ, biến toàn cục — nhưng
  **không được đặt tên hàm phụ là `main`**.

## Kênh thông tin duy nhất giữa Alice và Bob

| Thứ | Qua được vòng xoá không? | Ghi chú |
|---|---|---|
| Số viết trên sàn | ❌ Bị xoá sạch | Không dùng để giao tiếp Alice → Bob |
| Trạng thái đèn | ✅ Giữ nguyên | **Duy nhất** — nhưng Alice chỉ được flip ≤ 2 lần |
| Biến toàn cục / bộ nhớ | ❌ Hai luồng tài nguyên độc lập | Không share |

Vì vậy bản chất bài toán: **Alice flip nhiều nhất 2 bóng đèn** sao cho
Bob, xuất phát từ bất kỳ phòng nào, chỉ bằng việc đọc trạng thái đèn khi
đi lại trong mê cung, có thể xác định và dừng đúng ở phòng kho báu.
