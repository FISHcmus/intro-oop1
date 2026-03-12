# Hướng Dẫn Đồ Án Caro

**Môn:** Cơ sở lập trình — Khoa CNTT, ĐH KHTN TP.HCM
**Ngày:** 09/09/2024

## 1. Giới thiệu

Phối hợp các kỹ thuật và cấu trúc dữ liệu cơ bản để xây dựng trò chơi cờ caro đơn giản.

**Kiến thức cần có:** xử lý tập tin, handle, cấu trúc dữ liệu mảng một/hai chiều.

Phần hướng dẫn giúp xây dựng ở mức cơ bản — sinh viên tự nghiên cứu để hoàn thiện.

## 2. Kịch bản trò chơi

- Bàn cờ caro hiện trên màn hình
- Di chuyển bằng phím: **W** (lên), **A** (trái), **S** (xuống), **D** (phải)
- Nhấn **Enter** để đánh dấu: hiện `X` hoặc `O` tùy lượt
- Khi có người thắng → hiển thị chữ chúc mừng → hỏi tiếp tục (`Y`) hoặc thoát
- Khi bàn cờ kín → hiển thị "Hai bên hòa nhau" → hỏi tiếp tục/thoát
- Nhấn **ESC** (mã 27) để thoát game

```
Flowchart: Start → Chơi → Sự kiện? → Có → Tiếp tục? → Có/Không → End
```

## 3. Các bước xây dựng

### Bước 1: Cố định cửa sổ Console (View)

```cpp
void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}
```

Làm mờ nút maximize và không cho thay đổi kích thước cửa sổ.

### Bước 2: Hàm di chuyển con trỏ (View)

```cpp
void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
```

Dùng `_COORD` để thiết lập tọa độ trên màn hình console.

### Bước 3: Khai báo dữ liệu toàn cục

```cpp
#define BOARD_SIZE 12    // Kích thước ma trận bàn cờ
#define LEFT 3           // Tọa độ trái màn hình bàn cờ
#define TOP 1            // Tọa độ trên màn hình bàn cờ

struct _POINT {int x, y, c;};  // x: dòng, y: cột, c: đánh dấu
_POINT _A[BOARD_SIZE][BOARD_SIZE];  // Ma trận bàn cờ
bool _TURN;              // true = lượt người 1, false = lượt người 2
int _COMMAND;            // Giá trị phím người dùng nhập
int _X, _Y;              // Tọa độ hiện hành trên màn hình bàn cờ
```

**Quy ước `c`:** 0 = chưa đánh, -1 = lượt true đánh, 1 = lượt false đánh

### Bước 4: Khởi tạo dữ liệu (Model)

```cpp
void ResetData() {
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            _A[i][j].x = 4 * j + LEFT + 2;  // Hoành độ màn hình
            _A[i][j].y = 2 * i + TOP + 1;    // Tung độ màn hình
            _A[i][j].c = 0;                   // Chưa đánh
        }
    }
    _TURN = true; _COMMAND = -1;
    _X = _A[0][0].x; _Y = _A[0][0].y;
}
```

### Bước 5: Vẽ bàn cờ (View)

```cpp
void DrawBoard(int pSize){
    for(int i = 0; i <= pSize; i++){
        for(int j = 0; j <= pSize; j++){
            GotoXY(LEFT + 4 * i, TOP + 2 * j);
            printf(".");
        }
    }
}
```

### Bước 6: Hàm StartGame (Control)

```cpp
void StartGame() {
    system("cls");
    ResetData();             // Khởi tạo dữ liệu gốc
    DrawBoard(BOARD_SIZE);   // Vẽ màn hình game
}
```

```
Call flow: ResetData() → StartGame() → DrawBoard()
```

### Bước 7: ExitGame và GabageCollect (Control/Model)

```cpp
void GabageCollect() {
    // Dọn dẹp tài nguyên nếu có khai báo con trỏ
}

void ExitGame() {
    system("cls");
    GabageCollect();
    // Có thể lưu game trước khi exit
}
```

### Bước 8: Xử lý thắng/thua (View)

```cpp
int ProcessFinish(int pWhoWin) {
    GotoXY(0, _A[BOARD_SIZE-1][BOARD_SIZE-1].y + 2);
    switch(pWhoWin){
        case -1:
            printf("Nguoi choi %d da thang va nguoi choi %d da thua\n", true, false);
            break;
        case 1:
            printf("Nguoi choi %d da thang va nguoi choi %d da thua\n", false, true);
            break;
        case 0:
            printf("Nguoi choi %d da hoa nguoi choi %d\n", false, true);
            break;
        case 2:
            _TURN = !_TURN;  // Đổi lượt nếu không có gì xảy ra
    }
    GotoXY(_X, _Y);
    return pWhoWin;
}

int AskContinue() {
    GotoXY(0, _A[BOARD_SIZE-1][BOARD_SIZE-1].y + 4);
    printf("Nhan 'y/n' de tiep tuc/dung: ");
    return toupper(getch());
}
```

### Bước 9: Kiểm tra thắng/thua/hòa (Model)

```cpp
int TestBoard() {
    if(<Ma trận đầy>) return 0;          // Hòa
    else {
        if (<tồn tại điều kiện thắng theo luật caro>)
            return (_TURN == true ? -1 : 1);  // -1 = true thắng, 1 = false thắng
        else
            return 2;  // Chưa ai thắng
    }
}
```

**Quy ước trả về:** 0 = hòa, -1 = lượt true thắng, 1 = lượt false thắng, 2 = chưa ai thắng

### Bước 10: Đánh dấu bàn cờ (Model)

```cpp
int CheckBoard(int pX, int pY){
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            if(_A[i][j].x == pX && _A[i][j].y == pY && _A[i][j].c == 0){
                if(_TURN == true) _A[i][j].c = -1;
                else _A[i][j].c = 1;
                return _A[i][j].c;
            }
        }
    }
    return 0;
}
```

### Bước 11: Di chuyển trên bàn cờ (Control)

```cpp
void MoveRight() {
    if (_X < _A[BOARD_SIZE-1][BOARD_SIZE-1].x) {
        _X += 4;
        GotoXY(_X, _Y);
    }
}
void MoveLeft() {
    if (_X > _A[0][0].x) {
        _X -= 4;
        GotoXY(_X, _Y);
    }
}
void MoveDown() {
    if (_Y < _A[BOARD_SIZE-1][BOARD_SIZE-1].y) {
        _Y += 2;
        GotoXY(_X, _Y);
    }
}
void MoveUp() {
    if (_Y > _A[0][0].y) {
        _Y -= 2;
        GotoXY(_X, _Y);
    }
}
```

### Bước 12: Hàm main (Control)

```cpp
void main() {
    FixConsoleWindow();
    StartGame();
    bool validEnter = true;
    while (1) {
        _COMMAND = toupper(getch());
        if (_COMMAND == 27) {           // ESC
            ExitGame();
            return;
        }
        else {
            if (_COMMAND == 'A') MoveLeft();
            else if (_COMMAND == 'W') MoveUp();
            else if (_COMMAND == 'S') MoveDown();
            else if (_COMMAND == 'D') MoveRight();
            else if (_COMMAND == 13) {  // Enter
                switch(CheckBoard(_X, _Y)){
                    case -1: printf("X"); break;
                    case 1:  printf("O"); break;
                    case 0:  validEnter = false; break;
                }
                if(validEnter == true){
                    switch(ProcessFinish(TestBoard())){
                        case -1: case 1: case 0:
                            if(AskContinue() != 'Y'){
                                ExitGame(); return 0;
                            }
                            else StartGame();
                    }
                }
                validEnter = true;
            }
        }
    }
}
```

## 4. Yêu cầu đồ án

Phần hướng dẫn trên còn thiếu các chức năng cơ bản sau. Sinh viên cần bổ sung:

### 4.1 Xử lý lưu/tải trò chơi (save/load)
- Nhấn **'L'** → hiện prompt nhập tên tập tin → lưu trạng thái hiện hành
- Nhấn **'T'** → hiện prompt nhập tên tập tin → tải trạng thái đã lưu

### 4.2 Nhận biết thắng/thua/hòa
- Bổ sung tính năng kiểm tra quy luật thắng/thua/hòa trong caro
- Kiểm tra sau mỗi bước đi của người chơi

### 4.3 Xử lý hiệu ứng thắng/thua/hòa
- Hiện tại chỉ hiển thị dòng chữ báo hiệu đơn giản
- Cần cài đặt hiệu ứng sinh động hơn

### 4.4 Xử lý giao diện màn hình khi chơi
- Hiển thị thông số hai người chơi (số bước đã đánh, thua mấy ván...)
- Tổ chức giao diện rõ ràng, sinh động

### 4.5 Xử lý màn hình chính
- Hiển thị menu trước khi vào trò chơi: "New Game", "Load Game", "Settings"...
- Giúp chương trình hoàn thiện và giống thực tế hơn
