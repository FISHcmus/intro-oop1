#include "Renderer.h"
#include <cmath>

Renderer::Renderer()
    : camera({}), cellSize(50.0f), boardOriginX(0), boardOriginY(0),
      screenWidth(0), screenHeight(0) {}

Renderer::~Renderer() {}

void Renderer::init(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    // 2.5D camera setup — looking down at an angle
    camera.position = {6.0f, 10.0f, 6.0f};
    camera.target = {6.0f, 0.0f, 6.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // 2D fallback layout calculations
    float margin = 60.0f;
    float availableW = static_cast<float>(width) - 2.0f * margin - 200.0f; // HUD space
    float availableH = static_cast<float>(height) - 2.0f * margin;
    cellSize = std::min(availableW, availableH) / Board::SIZE;
    boardOriginX = margin;
    boardOriginY = margin;
}

void Renderer::shutdown() {}

void Renderer::updateCamera() {
    // TODO: smooth camera transitions (task 101)
}

void Renderer::drawBoard(const Board& board, int cursorRow, int cursorCol,
                          CellState currentTurn) {
    drawGrid();

    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            CellState cell = board.getCell(r, c);
            if (cell != CellState::Empty) {
                drawPiece(r, c, cell);
            }
        }
    }

    drawCursor(cursorRow, cursorCol, currentTurn);
}

void Renderer::drawWinLine(const std::vector<Move>& winLine) {
    for (const auto& m : winLine) {
        Vector2 pos = boardToScreen(m.row, m.col);
        float half = cellSize * 0.5f;
        DrawRectangle(static_cast<int>(pos.x - half),
                      static_cast<int>(pos.y - half),
                      static_cast<int>(cellSize),
                      static_cast<int>(cellSize),
                      Fade(GOLD, 0.4f));
    }
}

Vector2 Renderer::boardToScreen(int row, int col) const {
    float x = boardOriginX + static_cast<float>(col) * cellSize + cellSize * 0.5f;
    float y = boardOriginY + static_cast<float>(row) * cellSize + cellSize * 0.5f;
    return {x, y};
}

bool Renderer::screenToBoard(Vector2 screenPos, int& row, int& col) const {
    float relX = screenPos.x - boardOriginX;
    float relY = screenPos.y - boardOriginY;
    if (relX < 0 || relY < 0) return false;

    col = static_cast<int>(relX / cellSize);
    row = static_cast<int>(relY / cellSize);
    return (row >= 0 && row < Board::SIZE && col >= 0 && col < Board::SIZE);
}

void Renderer::drawGrid() {
    float boardSize = Board::SIZE * cellSize;
    Color gridColor = {80, 80, 80, 255};

    for (int i = 0; i <= Board::SIZE; i++) {
        float offset = static_cast<float>(i) * cellSize;
        // Horizontal lines
        DrawLineV({boardOriginX, boardOriginY + offset},
                  {boardOriginX + boardSize, boardOriginY + offset}, gridColor);
        // Vertical lines
        DrawLineV({boardOriginX + offset, boardOriginY},
                  {boardOriginX + offset, boardOriginY + boardSize}, gridColor);
    }
}

void Renderer::drawPiece(int row, int col, CellState state) {
    Vector2 center = boardToScreen(row, col);
    float radius = cellSize * 0.35f;

    if (state == CellState::PlayerX) {
        // Draw X with two crossed lines
        float offset = radius * 0.8f;
        DrawLineEx({center.x - offset, center.y - offset},
                   {center.x + offset, center.y + offset}, 3.0f, BLUE);
        DrawLineEx({center.x + offset, center.y - offset},
                   {center.x - offset, center.y + offset}, 3.0f, BLUE);
    } else if (state == CellState::PlayerO) {
        // Draw O as circle
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y),
                        radius, RED);
    }
}

void Renderer::drawCursor(int row, int col, CellState currentTurn) {
    Vector2 pos = boardToScreen(row, col);
    float half = cellSize * 0.45f;
    Color cursorColor = (currentTurn == CellState::PlayerX)
                            ? Fade(BLUE, 0.3f)
                            : Fade(RED, 0.3f);
    DrawRectangle(static_cast<int>(pos.x - half),
                  static_cast<int>(pos.y - half),
                  static_cast<int>(half * 2.0f),
                  static_cast<int>(half * 2.0f),
                  cursorColor);
}
