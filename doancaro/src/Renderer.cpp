#include "Renderer.h"
#include "raymath.h"
#include <cmath>

// Board sits on XZ plane at Y=0, cells [0..SIZE) on both axes
// Each cell is 1.0 world unit

Renderer::Renderer()
    : camera({}), cellSize(1.0f), screenWidth(0), screenHeight(0),
      hoverValid(false), hoverRow(0), hoverCol(0) {}

Renderer::~Renderer() = default;

void Renderer::init(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    float center = Board::SIZE / 2.0f;

    // Camera: above and behind the board, looking at center
    camera.position = {center, 14.0f, center + 10.0f};
    camera.target = {center, 0.0f, center};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void Renderer::shutdown() {}

void Renderer::updateCamera() {
    // Orbital camera: right-click drag to rotate, scroll to zoom
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);

    // Ray cast from mouse position to board plane for hover detection
    Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);

    // Board quad corners on Y=0 plane
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

void Renderer::drawBoard(const Board& board, int cursorRow, int cursorCol,
                          CellState currentTurn) {
    BeginMode3D(camera);

    drawBoardSurface();
    drawGrid3D();

    // Draw all placed pieces
    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            CellState cell = board.getCell(r, c);
            if (cell != CellState::Empty) {
                drawPiece3D(r, c, cell);
            }
        }
    }

    // Draw cursor/hover highlight
    if (hoverValid) {
        drawCursor3D(hoverRow, hoverCol, currentTurn);
    } else {
        // Fallback to keyboard cursor
        drawCursor3D(cursorRow, cursorCol, currentTurn);
    }

    EndMode3D();
}

void Renderer::drawWinLine(const std::vector<Move>& winLine) {
    BeginMode3D(camera);

    for (const auto& m : winLine) {
        float x = static_cast<float>(m.col) + 0.5f;
        float z = static_cast<float>(m.row) + 0.5f;
        // Golden highlight cube on winning cells
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
    // Wooden board surface
    DrawPlane({half, 0.0f, half},
              {static_cast<float>(Board::SIZE), static_cast<float>(Board::SIZE)},
              {60, 40, 20, 255}); // dark wood color
}

void Renderer::drawGrid3D() {
    Color gridColor = {140, 120, 90, 255}; // lighter wood grain lines

    for (int i = 0; i <= Board::SIZE; i++) {
        auto pos = static_cast<float>(i);
        auto boardLen = static_cast<float>(Board::SIZE);

        // Lines along X axis (horizontal when viewed from camera)
        DrawLine3D({0.0f, 0.01f, pos}, {boardLen, 0.01f, pos}, gridColor);
        // Lines along Z axis (vertical when viewed from camera)
        DrawLine3D({pos, 0.01f, 0.0f}, {pos, 0.01f, boardLen}, gridColor);
    }
}

void Renderer::drawPiece3D(int row, int col, CellState state) {
    float x = static_cast<float>(col) + 0.5f;
    float z = static_cast<float>(row) + 0.5f;

    if (state == CellState::PlayerX) {
        // X piece: blue cylinder (taller, thinner)
        DrawCylinder({x, 0.0f, z}, 0.0f, 0.3f, 0.4f, 8, {50, 120, 220, 255});
        DrawCylinderWires({x, 0.0f, z}, 0.0f, 0.3f, 0.4f, 8, {30, 80, 180, 255});
    } else if (state == CellState::PlayerO) {
        // O piece: red cylinder (wider, shorter disc)
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
