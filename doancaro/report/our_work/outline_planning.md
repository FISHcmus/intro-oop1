# Outline Planning — Báo cáo đồ án Caro

**Group:** Nguyễn Hữu Thiên Nhân (25310023), Bùi Thị Minh Hằng (25310057), Phạm Ngọc Trâm (25310043)
**Course:** Cơ sở lập trình (OOP1) — HCMUS, học kỳ 2 năm học 2025–2026
**Language:** Vietnamese
**Output file:** `doancaro/report/bao-cao-do-an-caro.md`
**Source project:** `doancaro/` (raylib 3D Caro, ~4233 LOC, 13 module)

---

## Guiding principles

1. **Dùng code của chính nhóm** — không sao chép ý từ báo cáo tham khảo của Nhóm 11 (SDL2). Chỉ mượn **hình dạng 5 chương**, không mượn nội dung.
2. **Giải thích thuật toán ở mức trừu tượng cao** — pseudocode, sơ đồ ASCII, mô tả bằng lời. Hạn chế dump full source code; chỉ trích đoạn khi thật sự cần minh họa.
3. **Chi tiết, cẩn thận** — không viết hời hợt. Có căn cứ từ file nguồn thật trong `doancaro/src/`.
4. **Văn phong học thuật tiếng Việt** — trang trọng, ngôi "nhóm chúng em", dùng thuật ngữ kỹ thuật nhất quán.

---

## Reference skeleton (mượn từ Nhóm 11, chỉ layout)

```
Bìa → Lời cảm ơn → Mục lục → Danh sách hình ảnh
Chương 1: Tổng quan + công cụ
Chương 2: Thiết kế hệ thống
Chương 3: Thuật toán
Chương 4: Đồ họa
Chương 5: Kết quả + đánh giá
Tài liệu tham khảo
```

---

## Chi tiết các chương (kế hoạch)

### Bìa
- Trường / Khoa / Bộ môn
- Tên đề tài: **Đồ án Caro**
- Nhóm: 3 thành viên (MSSV + họ tên)
- Lớp: *(chờ xác nhận)*
- GVHD: *(chờ xác nhận)*
- Ngày báo cáo: *(chờ xác nhận, mặc định 2026-04-22)*

### Lời cảm ơn
- 1 đoạn ngắn (~200 chữ), cảm ơn GVHD + các thành viên + raylib community.

### Mục lục + Danh sách hình ảnh
- Tự sinh từ các heading chính.

---

### Chương 1 — Tổng quan về đề tài và công cụ

**1.1 Giới thiệu đề tài Caro**
- Nguồn gốc, luật chơi (5 quân liên tiếp).
- Phiên bản của nhóm: bàn 15×15, 2 người chơi (PvP) hoặc Người–Máy (PvE).
- Chủ đề giao diện: bàn cờ 3D phong cách gỗ/đá Nhật Bản (Go-board aesthetic).

**1.2 Môi trường phát triển và công cụ**
- Ngôn ngữ: C++14 strict (theo yêu cầu môn học).
- Thư viện đồ họa: **raylib** (2D + 3D, input, audio, model GLB).
- Build: CMake 3.16+, FetchContent tự tải raylib.
- IDE: CLion (Linux) + Visual Studio 2022 (Windows) — đa nền tảng.
- Cấu trúc assets: `models/` (GLB 3D), `textures/` (PNG), `fonts/`, `sounds/`, `music/`, `pieces/`.

**1.3 Yêu cầu chức năng đã đạt được**
- Bảng checklist chính: PvP, PvE (3 mức khó), Save/Load 3 slot + autosave, Undo, Settings bền (`settings.cfg`), HUD, hiệu ứng 3D, âm thanh.

---

### Chương 2 — Thiết kế hệ thống

**2.1 Kiến trúc tổng thể**
- Sơ đồ phụ thuộc ASCII giữa 13 module:
  ```
  main.cpp → Game
  Game → { MenuScreen, SettingsScreen, SaveLoadScreen, GameScreen,
           Board, Player/AIPlayer, Renderer, ParticleSystem,
           AudioManager, FileManager, Fonts }
  ```
- Tóm tắt vai trò từng module (1 dòng/module).

**2.2 Máy trạng thái game**
- `GameState enum { Menu, Settings, SaveLoad, Playing, GameOver, Help, About }`.
- Sơ đồ chuyển trạng thái ASCII.
- Vòng lặp chính trong `Game::run()` — switch theo state.

**2.3 Mô hình bàn cờ và biểu diễn dữ liệu**
- `Board` class — mảng 15×15 `CellState`, `Move lastMove`, `int moveCount`.
- `CellState enum { Empty, X, O }`, `Move { row, col }`.
- Phương thức then chốt: `placeMove`, `undoMove`, `checkWinner`, `getCandidateMoves`, `countDirection`.

**2.4 Lớp Player và AIPlayer — kế thừa và đa hình**
- `Player` abstract: `getMark()`, `getMove(Board&) = 0` (pure virtual).
- `HumanPlayer` (setNextMove qua input), `AIPlayer` (tính toán).
- Minh họa **đa hình** qua `Game::currentPlayer->getMove(board)` mà không cần biết cụ thể human hay AI.

**2.5 Quản lý file Save/Load**
- **2.5.1 Định dạng file `.cfg` nhị phân** — struct layout (`SaveFileHeader`, board cells, stats).
- **2.5.2 Quét slot** — tìm `slot1.cfg`, `slot2.cfg`, `slot3.cfg` + `autosave.cfg`.
- **2.5.3 Save / Load / Delete** — pseudocode mỗi thao tác.
- **2.5.4 Autosave** — trigger sau mỗi nước đi, ghi đè file autosave.

**2.6 Luồng xử lý chính (Flowchart)**
- Flowchart ASCII từ `main()` → `Game::run()` → loop { pollInput, update, draw }.

---

### Chương 3 — Thuật toán trò chơi và AI

**3.1 Kiểm tra thắng (`Board::checkWinner`)**
- Nguyên lý: chỉ quét từ nước cuối cùng, 4 hướng (ngang / dọc / chéo chính / chéo phụ).
- `countDirection` đếm liên tiếp 2 phía, tổng = số quân cùng loại liên tiếp bao gồm cell gốc.
- Thắng khi tổng ≥ 5.
- Độ phức tạp O(1) mỗi lượt thay vì O(n²) quét toàn bàn.

**3.2 Sinh nước đi ứng viên**
- `getCandidateMoves(radius=2)` — chỉ xét các ô trống nằm trong bán kính Chebyshev 2 quanh các quân đã đặt.
- Lý do: bàn 15×15 có 225 ô nhưng branching factor thực tế ~20–30.
- Trường hợp biên: bàn trống → trả về ô trung tâm.

**3.3 Hàm đánh giá heuristic (Pattern Scoring)**
- Ý tưởng cao: trượt cửa sổ 5 ô quanh mỗi nước đi, tính điểm theo mẫu:
  - `FIVE` (thắng) = 1 000 000
  - `OPEN_FOUR` = 50 000
  - `FOUR`, `OPEN_THREE`, `THREE`, `OPEN_TWO`, `TWO` → thang điểm giảm dần
- Nhân hệ số phòng thủ cho mẫu của đối thủ (defense multiplier ~1.5).
- Tổng điểm = Σ điểm trên 4 hướng.

**3.4 AI Easy — Greedy One-Ply**
- **Không có tree search, không có TT.**
- Thuật toán:
  1. Lấy danh sách ứng viên (§3.2).
  2. Với mỗi ứng viên, tính `delta = localScore(after) − localScore(before)`.
  3. Trả về argmax(delta).
- Tại sao dùng **delta**, không phải điểm tuyệt đối?
  - Điểm tuyệt đối bỏ qua giá trị phòng thủ (chặn đối thủ): một ô chặn `OPEN_FOUR` đối thủ chỉ có `localAfter` nhỏ (quân đơn lẻ của AI), nhưng `localBefore` rất âm (mẫu đối thủ × 1.5).
  - Delta khôi phục ưu tiên đúng: chặn > tấn công yếu.
- Kết luận: Easy vẫn biết chặn mối đe dọa thắng ngay, nhưng mù đối với phản đòn của đối thủ (không lookahead).

**3.5 AI Normal / Hard — Minimax α-β + Transposition Table**
- Đệ quy Minimax với cắt tỉa α-β — nguyên lý cổ điển.
- **Move ordering:** sắp xếp ứng viên giảm dần theo điểm heuristic trước khi đệ quy → α-β cắt được sớm.
- **TT best-move hoisting:** nếu TT đã biết best move của nút này từ lần tìm trước → đưa lên đầu danh sách.
- **Transposition Table (Zobrist hashing):**
  - Mỗi vị trí bàn cờ có hash 64-bit.
  - TT lưu `{hash, depth, score, flag (exact/lower/upper), bestMove}`.
  - Early return khi trùng hash và depth ≥ yêu cầu.
- Độ sâu: Normal = 2, Hard = 3.
- Phân tích: Tại sao không đi sâu hơn? Depth 4 → ~120 s/nước (đã test trước đó).

**3.6 Undo và history stack**
- `Game::moveHistory` — stack của `MoveRecord { move, mark, prevLastMove }`.
- Undo = pop + `board.undoMove()` + đổi lượt + reset hoạt ảnh renderer.
- Tránh xoá hash/TT vì TT có key theo hash, tự khớp lại vị trí mới.

---

### Chương 4 — Kỹ thuật đồ họa và giao diện

**4.1 raylib — render 3D không shader tuỳ biến**
- raylib cung cấp sẵn `Camera3D`, `Model` (GLB), `DrawModel`, `DrawMesh`, phép chiếu phối cảnh.
- Pipeline: upload mesh → mỗi frame gọi `BeginMode3D(camera)` → draw board + pieces → `EndMode3D()` → overlay 2D UI.

**4.2 Menu chính (`MenuScreen`)**
- Danh sách lựa chọn: Play, Settings, Load, Help, About, Exit.
- Điều hướng phím mũi tên + Enter, hoặc chuột.
- ASCII mock-up giao diện.

**4.3 Màn hình chơi — camera orbit + HUD + debug panel**
- Camera quỹ đạo: chuột phải kéo → xoay; cuộn → zoom.
- HUD: tên người chơi, lượt hiện tại, số nước, thời gian chơi (`playTime`).
- Debug panel (phím F3): hiển thị thông số AI khi PvE — pre-score, minimax score, TT size, lý do chọn nước.

**4.4 Hoạt ảnh đặt quân**
- 3 thành phần:
  1. **Drop arc** — parabola rơi từ trên xuống trong 0.25 s.
  2. **Squash & stretch** — co giãn y-axis khi chạm bàn.
  3. **Shadow stretch** — bóng quân mở rộng rồi co lại theo y.
- Lưu state trong `Renderer::animations` (map cell → animation state).

**4.5 Hệ thống hạt (`ParticleSystem`)**
- 3 loại emit:
  - `emitPlacement` — tia sáng khi đặt quân (~12 hạt).
  - `emitLanding` — bụi khi quân chạm bàn.
  - `emitWinCelebration` — pháo hoa + sparkle dọc win line.
- Cấu trúc `Particle { pos, vel, size, lifetime, age, rot, rotSpeed, shimmerPhase }`.
- Update: `age += dt`, vel chịu trọng lực, alpha ~ (1 − age/lifetime).

**4.6 Hiệu ứng thắng**
- `drawWinLine` — 5 quân thắng được bao bởi glow phát sáng dần.
- `drawVignette` — tối 4 góc màn hình để tập trung.
- Trigger `emitWinCelebration` và `AudioManager::playWinSound`.

**4.7 Âm thanh (`AudioManager`)**
- raylib `Music` (background nhạc nền loop) + `Sound` (SFX click, place, win).
- Toggle bằng phím M.

---

### Chương 5 — Kết quả và hướng phát triển

**5.1 Kết quả thực nghiệm (screenshots)**
- Hình 5.1: Menu chính.
- Hình 5.2: Màn hình chơi PvP.
- Hình 5.3: Màn hình chơi PvE + debug panel (F3).
- Hình 5.4: Màn hình Settings (chọn mức khó AI).
- Hình 5.5: Màn hình Save/Load với 3 slot + autosave.
- Hình 5.6: Trạng thái GameOver + hiệu ứng win line.
- *(ảnh do nhóm chèn sau khi chụp; placeholder ở bản draft)*

**5.2 Tổng kết**
- Hoàn thành đủ yêu cầu bài.
- Điểm nổi bật: AI có 3 tier thực sự khác biệt (greedy vs minimax), 3D rendering, save/load binary gọn.

**5.3 Hạn chế và hướng phát triển**
- Hạn chế: depth AI tối đa 3 (trade-off thời gian/nước), chưa có tutorial, chưa có networking.
- Hướng phát triển: iterative deepening, opening book, online PvP qua socket, skin/theme.

**5.4 Phân công công việc**
- Bảng 3 cột: Thành viên | Công việc chính | Tỉ lệ đóng góp.
- *(chờ dữ liệu thực từ nhóm, có thể để trống để nhóm điền)*

---

### Tài liệu tham khảo

- raylib documentation (https://www.raylib.com)
- *Artificial Intelligence: A Modern Approach* — Russell & Norvig (Minimax, α-β)
- *Computer Gomoku: Principles and Practices* — Allis (heuristic scoring)
- C++ Reference (https://en.cppreference.com)
- Zobrist hashing — original 1970 paper / Wikipedia

---

## Điểm cần xác nhận với thầy hướng dẫn / nhóm

1. **Lớp (class code)** — CLAUDE.md chưa ghi, cần điền.
2. **GVHD (giảng viên OOP1)** — cần điền.
3. **Ngày báo cáo** — hiện dự kiến 2026-04-22; có thể đổi khi nộp.
4. **Screenshots** — dự định để placeholder `**Hình 5.x:** ...` trong draft, nhóm chèn PNG thật khi chụp.
5. **Phân công §5.4** — nhóm điền số liệu thực.
6. **Length target** — Nhóm 11 viết ~27 trang. Dự kiến bản mình ~25–30 trang sau khi export PDF (tuỳ nội dung).

---

## Order of writing (khi được duyệt outline)

1. Bìa + Lời cảm ơn + Mục lục
2. Chương 1
3. Chương 2 (2.1–2.6)
4. Chương 3 (3.1–3.6) — chương trọng tâm, viết kỹ nhất
5. Chương 4 (4.1–4.7)
6. Chương 5 + Tài liệu tham khảo
7. Rà soát + thêm danh sách hình ảnh cuối cùng

Mỗi chương viết xong → dừng, để nhóm đọc → sửa → viết chương tiếp.
