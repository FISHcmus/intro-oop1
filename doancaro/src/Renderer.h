#ifndef RENDERER_H
#define RENDERER_H

#include "Board.h"
#include "ParticleSystem.h"
#include "raylib.h"
#include <atomic>
#include <cmath>
#include <mutex>
#include <thread>
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
    void drawVignette();

    // Animation control
    void resetAnimations();

    // Camera update (call every frame)
    // Returns true on the frame a left-click first lands on any camera UI
    // button (rotate/zoom/reset). Caller plays a click SFX in response.
    bool updateCamera();
    void uploadPendingTextures();  // call each frame to upload bg-loaded images to GPU

    // Camera UI buttons (draw after EndMode3D, in 2D overlay)
    void drawCameraControls();

    // Game action buttons — returns true if clicked this frame
    bool drawSaveButton();
    bool drawLoadButton();
    bool drawMenuButton();
    bool drawSettingsButton();
    bool drawRestartButton();
    bool drawUndoButton();

    // Returns true if screen point is over a UI button (prevents board clicks)
    bool isPointOnUI(Vector2 point) const;

    // Coordinate conversion
    Vector2 boardToScreen(int row, int col) const;
    bool screenToBoard(Vector2 screenPos, int& row, int& col) const;

    // Mouse hover (updated each frame via ray cast)
    bool getHoveredCell(int& row, int& col) const;

    // Particle effects
    void updateParticles(float dt);
    void triggerWinParticles(const std::vector<Move>& winLine);

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
    Rectangle btnRestart;
    Rectangle btnUndo;

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
    bool winParticlesEmitted;

    // Particle system
    ParticleSystem particles;

    // 3D board model
    Model boardModel;
    bool boardModelLoaded;

    // Piece models (sphere mesh + procedural wood textures)
    Model pieceModelLight;  // PlayerX — light maple wood
    Model pieceModelDark;   // PlayerO — dark walnut wood
    bool pieceModelsLoaded;

    // Per-position unique textures (each piece has distinct wood grain)
    Texture2D pieceTexLight[Board::SIZE][Board::SIZE];
    Texture2D pieceTexDark[Board::SIZE][Board::SIZE];
    Texture2D defaultTexLight;  // fallback while async loading
    Texture2D defaultTexDark;
    bool pieceTexReady[Board::SIZE][Board::SIZE];

    // Async texture loading
    Image pieceImgLight[Board::SIZE][Board::SIZE];
    Image pieceImgDark[Board::SIZE][Board::SIZE];
    std::atomic<int> imagesLoaded;  // count of images loaded by bg thread
    std::thread texLoaderThread;
    int texUploadIndex;  // next index to upload to GPU

    // Glossy Phong lighting shader for pieces
    Shader glossShader;
    bool glossShaderLoaded;
    int glossViewPosLoc;

    // Matte Phong lighting shader for board
    Shader matteShader;
    bool matteShaderLoaded;
    int matteViewPosLoc;

    // Ground plane
    Model groundModel;
    bool groundLoaded;

    // Animation constants
    static constexpr float PIECE_ANIM_DURATION = 0.7f;
    static constexpr float LAST_MOVE_PULSE_SPEED = 3.0f;
    static constexpr float WIN_PULSE_SPEED = 3.0f;

    // Camera helpers
    void rebuildCameraFromOrbit();
    void handleRightClickDrag();
    void handleGestures();
    void handleScrollZoom();
    bool isPointOnCameraButton(Vector2 point) const;

    // 3D drawing helpers
    // drawGrid3D removed — using baked grid from Go board model
    void drawPiece3D(int row, int col, CellState state, float anim);
    static void drawCursor3D(int row, int col, CellState currentTurn);
    void drawBoardSurface();
    static void drawLastMoveIndicator(int row, int col, float time);

    // UI helper
    static bool drawButton(Rectangle rect, const char* label, int fontSize);
};

#endif
