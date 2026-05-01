#ifndef RENDERER_H
#define RENDERER_H

#include "Board.h"
#include "ParticleSystem.h"
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
    void drawMist();
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

    // Idle camera-breathing — sin-wave Y bob applied AFTER orbit rebuild
    // when no input has occurred for ≥1.5s. Cosmetic only; never mutates
    // cameraTarget so it can't drift over time. breathTime keeps phase
    // continuous so resuming idle doesn't snap (which would teleport).
    float idleSeconds;
    float breathTime;

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

    // Sơn/Thủy talisman GLB pieces.
    Model pieceModelSon;    // PlayerX — fire rune
    Model pieceModelThuy;   // PlayerO — water rune
    float pieceScaleSon;    // bbox-derived fit-to-cell scalar
    float pieceScaleThuy;
    bool  pieceGLBLoaded;

    // Glossy Phong wet-coat shader — applied to pieces, board, and the
    // floating-island rock. Single shader keeps highlight direction and
    // shininess consistent across every lit surface in the scene.
    Shader glossShader;
    bool glossShaderLoaded;
    int glossViewPosLoc;
    // Rim-light uniform locations — pushed ON before rock draw, reset to OFF
    // afterwards so pieces/board never get the glow.
    int glossRimStrengthLoc;
    int glossRimPulseLoc;

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

    // Drifting mist — multi-octave value-noise FS shader. UV scrolled by a
    // time uniform so the noise field pans along a wind vector. Drawn on a
    // fullscreen 2D quad between the 3D scene and the vignette pass.
    Shader mistShader;
    bool   mistShaderLoaded;
    int    mistTimeLoc;

    // 3D fluffy clouds — single GLB mesh instanced multiple times at
    // varied positions, scales, and drift speeds, all kept BELOW the
    // board's Y so the play area stays in clear sky.
    struct CloudInstance {
        Vector3 pos;        // world-space center
        float   scale;      // uniform XYZ scale
        float   driftSpeed; // X velocity (units/sec)
    };
    std::vector<CloudInstance> cloudInstances;
    Model  cloudModel;
    bool   cloudModelLoaded;


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
