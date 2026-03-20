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

    // Board rendering
    void drawBoard(const Board& board, int cursorRow, int cursorCol,
                   CellState currentTurn);
    void drawWinLine(const std::vector<Move>& winLine);

    // Camera
    void updateCamera();

    // Helpers
    Vector2 boardToScreen(int row, int col) const;
    bool screenToBoard(Vector2 screenPos, int& row, int& col) const;

    float getCellSize() const { return cellSize; }

private:
    Camera3D camera;
    float cellSize;
    float boardOriginX;
    float boardOriginY;
    int screenWidth;
    int screenHeight;

    void drawGrid();
    void drawPiece(int row, int col, CellState state);
    void drawCursor(int row, int col, CellState currentTurn);
};

#endif
