#ifndef RENDERER_H
#define RENDERER_H

#include "Board.h"
#include "ParticleSystem.h"
#include "raylib.h"
#include <atomic>
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

    // 2D backdrop — 3-stop ink-wash sky drawn full-screen via inline shader.
    // Call BEFORE BeginMode3D so the 3D scene draws on top.
    void drawSkyGradient();

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

    // Sơn/Thủy talisman GLBs override the sphere fallback when loaded.
    Model pieceModelSon;    // PlayerX — fire rune
    Model pieceModelThuy;   // PlayerO — water rune
    float pieceScaleSon;    // bbox-derived fit-to-cell scalar
    float pieceScaleThuy;
    bool  pieceGLBLoaded;

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

    // Glossy Phong wet-coat shader — applied to pieces, board, and the
    // floating-island rock. Single shader keeps highlight direction and
    // shininess consistent across every lit surface in the scene.
    Shader glossShader;
    bool glossShaderLoaded;
    int glossViewPosLoc;

    // Sky-gradient shader — 3-stop smoothstep interpolation between
    // sky_top / sky_mid / sky_horizon. Single fullscreen quad, no 3D state,
    // no textures. Kills the Mach-band seam that two stacked
    // DrawRectangleGradientV calls produce.
    Shader skyShader;
    bool   skyShaderLoaded;
    int    skyTopLoc;
    int    skyMidLoc;
    int    skyBotLoc;

    // Edge-fade shader applied to the scroll backdrop. Multiplies fragment
    // alpha by 1 - smoothstep(fadeStart, 1.0, ellipseDist) so the scroll's
    // hard rectangular outer edge fades into the gradient sky behind it.
    // All uniforms are static post-init — pushed once in init(), never per-frame.
    Shader edgeFadeShader;
    bool   edgeFadeShaderLoaded;

    // Fullscreen vignette — radial smoothstep on screen-space distance from
    // center darkens the corners. Drawn over the 3D scene, under the HUD,
    // so labels stay readable while the periphery dims into ink.
    Shader vignetteShader;
    bool   vignetteShaderLoaded;

    // 3D backdrop — Sketchfab "mountain & river scroll" (KHR_materials_unlit,
    // vertex-colored anime style). Sits behind the play area; do NOT apply
    // glossShader to it — model is already unlit.
    Model scrollModel;
    bool  scrollLoaded;
    float scrollScale;
    // X/Z offset to apply when drawing so the model's bbox center sits at the
    // requested world position. Computed once at load — auto-recenters when
    // kBackdropWidth changes, no manual position math needed.
    Vector3 scrollCenterOffset;

    // Floating-island pedestal — sits directly under the board so the play
    // area reads as resting on a chunk of levitating stone rather than hanging
    // in mid-air. Same bbox-fit / auto-center pattern as the scroll backdrop.
    Model   islandModel;
    bool    islandLoaded;
    float   islandScale;
    Vector3 islandCenterOffset;

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
