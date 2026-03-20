#ifndef RENDERER_H
#define RENDERER_H

#include "Board.h"
#include "raylib.h"
#include <cmath>
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

    // Animation control
    void resetAnimations();

    // Camera update (call every frame)
    void updateCamera();

    // Camera UI buttons (draw after EndMode3D, in 2D overlay)
    void drawCameraControls();

    // Game action buttons — returns true if clicked this frame
    bool drawSaveButton();
    bool drawLoadButton();
    bool drawMenuButton();
    bool drawSettingsButton();

    // Returns true if screen point is over a UI button (prevents board clicks)
    bool isPointOnUI(Vector2 point) const;

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

    // Orbital camera state
    float cameraAngle;    // horizontal orbit angle (radians)
    float cameraPitch;    // vertical pitch angle (radians)
    float cameraDistance;  // distance from target
    Vector3 cameraTarget; // look-at point (board center)

    // Default camera values (for reset)
    float defaultAngle;
    float defaultPitch;
    float defaultDistance;

    // Right-click drag state
    bool isDragging;
    Vector2 dragStart;

    // UI button rectangles
    Rectangle btnRotateLeft;
    Rectangle btnRotateRight;
    Rectangle btnZoomIn;
    Rectangle btnZoomOut;
    Rectangle btnReset;
    Rectangle btnSave;
    Rectangle btnLoad;
    Rectangle btnMenu;
    Rectangle btnSettings;

    // Cached hover from ray cast
    bool hoverValid;
    int hoverRow;
    int hoverCol;

    // Animation state
    CellState prevCells[Board::SIZE][Board::SIZE];
    float pieceAnimStart[Board::SIZE][Board::SIZE];
    Move lastMove;
    float winLineStart;
    bool showingWinLine;

    // Animation constants
    static constexpr float PIECE_ANIM_DURATION = 0.3f;
    static constexpr float LAST_MOVE_PULSE_SPEED = 3.0f;
    static constexpr float WIN_PULSE_SPEED = 3.0f;

    // Camera helpers
    void rebuildCameraFromOrbit();
    void handleRightClickDrag();
    void handleGestures();
    void handleScrollZoom();

    // 3D drawing helpers
    static void drawGrid3D();
    static void drawPiece3D(int row, int col, CellState state, float anim);
    static void drawCursor3D(int row, int col, CellState currentTurn);
    static void drawBoardSurface();
    static void drawLastMoveIndicator(int row, int col, float time);

    // UI helper
    static bool drawButton(Rectangle rect, const char* label, int fontSize);
};

#endif
