#include "Renderer.h"
#include "raymath.h"
#include <cmath>

// Board sits on XZ plane at Y=0, cells [0..SIZE) on both axes
// Each cell is 1.0 world unit

static const float ORBIT_SPEED = 0.005f;     // radians per pixel of drag
static const float BUTTON_ORBIT_SPEED = 0.05f; // radians per frame when button held
static const float ZOOM_SPEED = 0.5f;
static const float MIN_DISTANCE = 8.0f;
static const float MAX_DISTANCE = 30.0f;
static const float MIN_PITCH = 0.2f;          // ~11 degrees (don't go below board)
static const float MAX_PITCH = 1.4f;          // ~80 degrees (nearly top-down)
static const int BTN_SIZE = 36;
static const int BTN_PAD = 6;

Renderer::Renderer()
    : camera({}), cellSize(1.0f), screenWidth(0), screenHeight(0),
      cameraAngle(0), cameraPitch(0), cameraDistance(0),
      cameraTarget({}), defaultAngle(0), defaultPitch(0), defaultDistance(0),
      isDragging(false), dragStart({}),
      btnRotateLeft({}), btnRotateRight({}), btnZoomIn({}), btnZoomOut({}), btnReset({}),
      btnSave({}), btnLoad({}),
      hoverValid(false), hoverRow(0), hoverCol(0) {}

Renderer::~Renderer() = default;

void Renderer::init(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    float center = Board::SIZE / 2.0f;
    cameraTarget = {center, 0.0f, center};

    // Default orbital parameters
    defaultAngle = 0.0f;          // looking from front
    defaultPitch = 0.85f;         // ~49 degrees down
    defaultDistance = 16.0f;

    cameraAngle = defaultAngle;
    cameraPitch = defaultPitch;
    cameraDistance = defaultDistance;

    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    rebuildCameraFromOrbit();

    // Layout UI buttons in bottom-left corner
    float bx = BTN_PAD;
    float by = static_cast<float>(height) - BTN_SIZE - BTN_PAD;

    btnRotateLeft  = {bx, by, BTN_SIZE, BTN_SIZE};
    btnRotateRight = {bx + BTN_SIZE + BTN_PAD, by, BTN_SIZE, BTN_SIZE};
    btnZoomIn      = {bx + 2 * (BTN_SIZE + BTN_PAD), by, BTN_SIZE, BTN_SIZE};
    btnZoomOut     = {bx + 3 * (BTN_SIZE + BTN_PAD), by, BTN_SIZE, BTN_SIZE};
    btnReset       = {bx + 4 * (BTN_SIZE + BTN_PAD), by, BTN_SIZE + 20, BTN_SIZE};

    // Save/Load buttons — top-left
    btnSave = {BTN_PAD, BTN_PAD, 60, BTN_SIZE};
    btnLoad = {BTN_PAD + 60 + BTN_PAD, BTN_PAD, 60, BTN_SIZE};
}

void Renderer::shutdown() {}

void Renderer::rebuildCameraFromOrbit() {
    // Convert spherical coordinates to camera position
    float x = cameraTarget.x + cameraDistance * cosf(cameraPitch) * sinf(cameraAngle);
    float y = cameraTarget.y + cameraDistance * sinf(cameraPitch);
    float z = cameraTarget.z + cameraDistance * cosf(cameraPitch) * cosf(cameraAngle);

    camera.position = {x, y, z};
    camera.target = cameraTarget;
}

void Renderer::handleRightClickDrag() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        isDragging = true;
        dragStart = GetMousePosition();
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        isDragging = false;
    }
    if (isDragging && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 current = GetMousePosition();
        float dx = current.x - dragStart.x;
        float dy = current.y - dragStart.y;

        cameraAngle -= dx * ORBIT_SPEED;
        cameraPitch += dy * ORBIT_SPEED;

        // Clamp pitch
        if (cameraPitch < MIN_PITCH) cameraPitch = MIN_PITCH;
        if (cameraPitch > MAX_PITCH) cameraPitch = MAX_PITCH;

        dragStart = current;
        rebuildCameraFromOrbit();
    }
}

void Renderer::handleGestures() {
    // Two-finger drag for orbit on touch devices
    if (GetTouchPointCount() >= 2) {
        int gesture = GetGestureDetected();
        if (gesture == GESTURE_PINCH_IN) {
            cameraDistance += ZOOM_SPEED;
            if (cameraDistance > MAX_DISTANCE) cameraDistance = MAX_DISTANCE;
            rebuildCameraFromOrbit();
        } else if (gesture == GESTURE_PINCH_OUT) {
            cameraDistance -= ZOOM_SPEED;
            if (cameraDistance < MIN_DISTANCE) cameraDistance = MIN_DISTANCE;
            rebuildCameraFromOrbit();
        }
    }
}

void Renderer::handleScrollZoom() {
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        cameraDistance -= wheel * ZOOM_SPEED * 2.0f;
        if (cameraDistance < MIN_DISTANCE) cameraDistance = MIN_DISTANCE;
        if (cameraDistance > MAX_DISTANCE) cameraDistance = MAX_DISTANCE;
        rebuildCameraFromOrbit();
    }
}

void Renderer::updateCamera() {
    // Process camera inputs
    handleRightClickDrag();
    handleGestures();
    handleScrollZoom();

    // Process button inputs (held = continuous rotation)
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, btnRotateLeft)) {
            cameraAngle += BUTTON_ORBIT_SPEED;
            rebuildCameraFromOrbit();
        }
        if (CheckCollisionPointRec(mouse, btnRotateRight)) {
            cameraAngle -= BUTTON_ORBIT_SPEED;
            rebuildCameraFromOrbit();
        }
        if (CheckCollisionPointRec(mouse, btnZoomIn)) {
            cameraDistance -= ZOOM_SPEED * 0.3f;
            if (cameraDistance < MIN_DISTANCE) cameraDistance = MIN_DISTANCE;
            rebuildCameraFromOrbit();
        }
        if (CheckCollisionPointRec(mouse, btnZoomOut)) {
            cameraDistance += ZOOM_SPEED * 0.3f;
            if (cameraDistance > MAX_DISTANCE) cameraDistance = MAX_DISTANCE;
            rebuildCameraFromOrbit();
        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, btnReset)) {
            cameraAngle = defaultAngle;
            cameraPitch = defaultPitch;
            cameraDistance = defaultDistance;
            rebuildCameraFromOrbit();
        }
    }

    // Keyboard shortcuts: Q/E rotate, Home reset
    if (IsKeyDown(KEY_Q)) {
        cameraAngle += BUTTON_ORBIT_SPEED;
        rebuildCameraFromOrbit();
    }
    if (IsKeyDown(KEY_E)) {
        cameraAngle -= BUTTON_ORBIT_SPEED;
        rebuildCameraFromOrbit();
    }
    if (IsKeyPressed(KEY_HOME)) {
        cameraAngle = defaultAngle;
        cameraPitch = defaultPitch;
        cameraDistance = defaultDistance;
        rebuildCameraFromOrbit();
    }

    // Ray cast for hover detection
    Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);

    Vector3 g0 = {0.0f, 0.0f, 0.0f};
    Vector3 g1 = {static_cast<float>(Board::SIZE), 0.0f, 0.0f};
    Vector3 g2 = {static_cast<float>(Board::SIZE), 0.0f, static_cast<float>(Board::SIZE)};
    Vector3 g3 = {0.0f, 0.0f, static_cast<float>(Board::SIZE)};

    RayCollision hit = GetRayCollisionQuad(ray, g0, g1, g2, g3);
    if (hit.hit) {
        int col = static_cast<int>(hit.point.x);
        int row = static_cast<int>(hit.point.z);
        if (row >= 0 && row < Board::SIZE && col >= 0 && col < Board::SIZE) {
            hoverValid = true;
            hoverRow = row;
            hoverCol = col;
        } else {
            hoverValid = false;
        }
    } else {
        hoverValid = false;
    }
}

bool Renderer::isPointOnUI(Vector2 point) const {
    return CheckCollisionPointRec(point, btnRotateLeft)
        || CheckCollisionPointRec(point, btnRotateRight)
        || CheckCollisionPointRec(point, btnZoomIn)
        || CheckCollisionPointRec(point, btnZoomOut)
        || CheckCollisionPointRec(point, btnReset)
        || CheckCollisionPointRec(point, btnSave)
        || CheckCollisionPointRec(point, btnLoad);
}

bool Renderer::drawButton(Rectangle rect, const char* label, int fontSize) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    bool held = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    Color bg = held ? Color{80, 80, 80, 220} : (hovered ? Color{70, 70, 70, 200} : Color{50, 50, 50, 180});
    Color border = hovered ? WHITE : Color{150, 150, 150, 200};

    DrawRectangleRec(rect, bg);
    DrawRectangleLinesEx(rect, 1.0f, border);

    int tw = MeasureText(label, fontSize);
    auto tx = static_cast<int>(rect.x + (rect.width - static_cast<float>(tw)) / 2.0f);
    auto ty = static_cast<int>(rect.y + (rect.height - static_cast<float>(fontSize)) / 2.0f);
    DrawText(label, tx, ty, fontSize, WHITE);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void Renderer::drawCameraControls() {
    drawButton(btnRotateLeft, "<", 20);
    drawButton(btnRotateRight, ">", 20);
    drawButton(btnZoomIn, "+", 20);
    drawButton(btnZoomOut, "-", 20);
    drawButton(btnReset, "Reset", 14);
}

bool Renderer::drawSaveButton() {
    return drawButton(btnSave, "Save", 14);
}

bool Renderer::drawLoadButton() {
    return drawButton(btnLoad, "Load", 14);
}

void Renderer::drawBoard(const Board& board, int cursorRow, int cursorCol,
                          CellState currentTurn) {
    BeginMode3D(camera);

    drawBoardSurface();
    drawGrid3D();

    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            CellState cell = board.getCell(r, c);
            if (cell != CellState::Empty) {
                drawPiece3D(r, c, cell);
            }
        }
    }

    if (hoverValid) {
        drawCursor3D(hoverRow, hoverCol, currentTurn);
    } else {
        drawCursor3D(cursorRow, cursorCol, currentTurn);
    }

    EndMode3D();
}

void Renderer::drawWinLine(const std::vector<Move>& winLine) {
    BeginMode3D(camera);

    for (const auto& m : winLine) {
        float x = static_cast<float>(m.col) + 0.5f;
        float z = static_cast<float>(m.row) + 0.5f;
        DrawCube({x, 0.02f, z}, 0.95f, 0.04f, 0.95f, Fade(GOLD, 0.6f));
        DrawCubeWires({x, 0.02f, z}, 0.95f, 0.04f, 0.95f, GOLD);
    }

    EndMode3D();
}

Vector2 Renderer::boardToScreen(int row, int col) const {
    Vector3 worldPos = {static_cast<float>(col) + 0.5f, 0.0f,
                        static_cast<float>(row) + 0.5f};
    return GetWorldToScreen(worldPos, camera);
}

bool Renderer::screenToBoard(Vector2 screenPos, int& row, int& col) const {
    Ray ray = GetScreenToWorldRay(screenPos, camera);

    Vector3 g0 = {0.0f, 0.0f, 0.0f};
    Vector3 g1 = {static_cast<float>(Board::SIZE), 0.0f, 0.0f};
    Vector3 g2 = {static_cast<float>(Board::SIZE), 0.0f, static_cast<float>(Board::SIZE)};
    Vector3 g3 = {0.0f, 0.0f, static_cast<float>(Board::SIZE)};

    RayCollision hit = GetRayCollisionQuad(ray, g0, g1, g2, g3);
    if (!hit.hit) return false;

    col = static_cast<int>(hit.point.x);
    row = static_cast<int>(hit.point.z);
    return (row >= 0 && row < Board::SIZE && col >= 0 && col < Board::SIZE);
}

bool Renderer::getHoveredCell(int& row, int& col) const {
    if (!hoverValid) return false;
    row = hoverRow;
    col = hoverCol;
    return true;
}

// --- Private 3D drawing helpers ---

void Renderer::drawBoardSurface() {
    float half = Board::SIZE / 2.0f;
    DrawPlane({half, 0.0f, half},
              {static_cast<float>(Board::SIZE), static_cast<float>(Board::SIZE)},
              {60, 40, 20, 255});
}

void Renderer::drawGrid3D() {
    Color gridColor = {140, 120, 90, 255};

    for (int i = 0; i <= Board::SIZE; i++) {
        auto pos = static_cast<float>(i);
        auto boardLen = static_cast<float>(Board::SIZE);

        DrawLine3D({0.0f, 0.01f, pos}, {boardLen, 0.01f, pos}, gridColor);
        DrawLine3D({pos, 0.01f, 0.0f}, {pos, 0.01f, boardLen}, gridColor);
    }
}

void Renderer::drawPiece3D(int row, int col, CellState state) {
    float x = static_cast<float>(col) + 0.5f;
    float z = static_cast<float>(row) + 0.5f;

    if (state == CellState::PlayerX) {
        DrawCylinder({x, 0.0f, z}, 0.0f, 0.3f, 0.4f, 8, {50, 120, 220, 255});
        DrawCylinderWires({x, 0.0f, z}, 0.0f, 0.3f, 0.4f, 8, {30, 80, 180, 255});
    } else if (state == CellState::PlayerO) {
        DrawCylinder({x, 0.0f, z}, 0.35f, 0.35f, 0.2f, 16, {220, 60, 60, 255});
        DrawCylinderWires({x, 0.0f, z}, 0.35f, 0.35f, 0.2f, 16, {180, 30, 30, 255});
    }
}

void Renderer::drawCursor3D(int row, int col, CellState currentTurn) {
    float x = static_cast<float>(col) + 0.5f;
    float z = static_cast<float>(row) + 0.5f;

    Color cursorColor = (currentTurn == CellState::PlayerX)
                            ? Fade(BLUE, 0.25f)
                            : Fade(RED, 0.25f);
    Color wireColor = (currentTurn == CellState::PlayerX)
                          ? Fade(BLUE, 0.5f)
                          : Fade(RED, 0.5f);

    DrawCube({x, 0.025f, z}, 0.9f, 0.05f, 0.9f, cursorColor);
    DrawCubeWires({x, 0.025f, z}, 0.9f, 0.05f, 0.9f, wireColor);
}
