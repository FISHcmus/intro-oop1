#ifndef RENDERER_H
#define RENDERER_H

#include "Board.h"
#include "raylib.h"
#include <vector>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init(int screenWidth, int screenHeight);
    void shutdown();

    // Board rendering (3D)
    void drawBoard(const Board& board, int cursorRow, int cursorCol,
                   CellState currentTurn);
    void drawWinLine(const std::vector<Move>& winLine);

    // Camera
    void updateCamera();

    // Coordinate conversion
    Vector2 boardToScreen(int row, int col) const;
    bool screenToBoard(Vector2 screenPos, int& row, int& col) const;

    // Mouse hover (updated each frame via ray cast)
    bool getHoveredCell(int& row, int& col) const;

    float getCellSize() const { return cellSize; }
    Camera3D getCamera() const { return camera; }

private:
    Camera3D camera;
    float cellSize;       // world units per cell (1.0)
    int screenWidth;
    int screenHeight;

    // Cached hover from ray cast
    bool hoverValid;
    int hoverRow;
    int hoverCol;

    // 3D drawing helpers
    static void drawGrid3D();
    static void drawPiece3D(int row, int col, CellState state);
    static void drawCursor3D(int row, int col, CellState currentTurn);
    static void drawBoardSurface();
};

#endif
