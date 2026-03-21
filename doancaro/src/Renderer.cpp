#include "Renderer.h"
#include "Fonts.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>

// Embedded Phong vertex shader (GLSL 330)
static const char* glossVS =
    "#version 330\n"
    "in vec3 vertexPosition;\n"
    "in vec2 vertexTexCoord;\n"
    "in vec3 vertexNormal;\n"
    "uniform mat4 mvp;\n"
    "uniform mat4 matModel;\n"
    "uniform mat4 matNormal;\n"
    "out vec3 fragPosition;\n"
    "out vec2 fragTexCoord;\n"
    "out vec3 fragNormal;\n"
    "void main() {\n"
    "    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));\n"
    "    fragTexCoord = vertexTexCoord;\n"
    "    fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));\n"
    "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
    "}\n";

// Embedded Phong fragment shader — glossy/wet look with strong specular
static const char* glossFS =
    "#version 330\n"
    "in vec3 fragPosition;\n"
    "in vec2 fragTexCoord;\n"
    "in vec3 fragNormal;\n"
    "uniform sampler2D texture0;\n"
    "uniform vec4 colDiffuse;\n"
    "uniform vec3 viewPos;\n"
    "out vec4 finalColor;\n"
    "void main() {\n"
    "    vec3 lightPos = vec3(10.0, 20.0, 10.0);\n"
    "    vec3 lightColor = vec3(1.0, 0.98, 0.95);\n"
    "    float ambientStrength = 0.35;\n"
    "    float specularStrength = 0.8;\n"
    "    float shininess = 64.0;\n"
    "    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse;\n"
    "    vec3 norm = normalize(fragNormal);\n"
    "    vec3 lightDir = normalize(lightPos - fragPosition);\n"
    // Diffuse
    "    float diff = max(dot(norm, lightDir), 0.0);\n"
    // Specular (Blinn-Phong)
    "    vec3 viewDir = normalize(viewPos - fragPosition);\n"
    "    vec3 halfDir = normalize(lightDir + viewDir);\n"
    "    float spec = pow(max(dot(norm, halfDir), 0.0), shininess);\n"
    // Combine
    "    vec3 ambient = ambientStrength * lightColor;\n"
    "    vec3 diffuse = diff * lightColor;\n"
    "    vec3 specular = specularStrength * spec * lightColor;\n"
    // Second fill light from opposite side (softer)
    "    vec3 lightPos2 = vec3(-5.0, 12.0, -5.0);\n"
    "    vec3 lightDir2 = normalize(lightPos2 - fragPosition);\n"
    "    float diff2 = max(dot(norm, lightDir2), 0.0) * 0.3;\n"
    "    vec3 halfDir2 = normalize(lightDir2 + viewDir);\n"
    "    float spec2 = pow(max(dot(norm, halfDir2), 0.0), shininess) * 0.3;\n"
    "    diffuse += diff2 * lightColor;\n"
    "    specular += specularStrength * spec2 * lightColor;\n"
    "    vec3 result = (ambient + diffuse) * texColor.rgb + specular;\n"
    "    finalColor = vec4(result, texColor.a);\n"
    "}\n";

// Matte wood shader for the board — low specular, soft diffuse
static const char* matteFS =
    "#version 330\n"
    "in vec3 fragPosition;\n"
    "in vec2 fragTexCoord;\n"
    "in vec3 fragNormal;\n"
    "uniform sampler2D texture0;\n"
    "uniform vec4 colDiffuse;\n"
    "uniform vec3 viewPos;\n"
    "out vec4 finalColor;\n"
    "void main() {\n"
    "    vec3 lightPos = vec3(10.0, 20.0, 10.0);\n"
    "    vec3 lightColor = vec3(1.0, 0.98, 0.95);\n"
    "    float ambientStrength = 0.4;\n"
    "    float specularStrength = 0.12;\n"
    "    float shininess = 8.0;\n"
    "    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse;\n"
    "    vec3 norm = normalize(fragNormal);\n"
    "    vec3 lightDir = normalize(lightPos - fragPosition);\n"
    "    float diff = max(dot(norm, lightDir), 0.0);\n"
    "    vec3 viewDir = normalize(viewPos - fragPosition);\n"
    "    vec3 halfDir = normalize(lightDir + viewDir);\n"
    "    float spec = pow(max(dot(norm, halfDir), 0.0), shininess);\n"
    "    vec3 ambient = ambientStrength * lightColor;\n"
    "    vec3 diffuse = diff * lightColor;\n"
    "    vec3 specular = specularStrength * spec * lightColor;\n"
    "    vec3 lightPos2 = vec3(-5.0, 12.0, -5.0);\n"
    "    vec3 lightDir2 = normalize(lightPos2 - fragPosition);\n"
    "    float diff2 = max(dot(norm, lightDir2), 0.0) * 0.3;\n"
    "    vec3 halfDir2 = normalize(lightDir2 + viewDir);\n"
    "    float spec2 = pow(max(dot(norm, halfDir2), 0.0), shininess) * 0.3;\n"
    "    diffuse += diff2 * lightColor;\n"
    "    specular += specularStrength * spec2 * lightColor;\n"
    "    vec3 result = (ambient + diffuse) * texColor.rgb + specular;\n"
    "    finalColor = vec4(result, texColor.a);\n"
    "}\n";

// Board sits on XZ plane at Y=0, cells [0..SIZE) on both axes
// Each cell is 1.0 world unit

static const float ORBIT_SPEED = 0.005f;     // radians per pixel of drag
static const float BUTTON_ORBIT_SPEED = 0.05f; // radians per frame when button held
static const float ZOOM_SPEED = 0.5f;
static const float MIN_DISTANCE = 8.0f;
static const float MAX_DISTANCE = 45.0f;
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
      btnSave({}), btnLoad({}), btnMenu({}), btnSettings({}), btnRestart({}),
      hoverValid(false), hoverRow(0), hoverCol(0),
      prevCells{}, pieceAnimStart{}, lastMove{-1, -1},
      winLineStart(0.0f), showingWinLine(false),
      winParticlesEmitted(false),
      boardModel({}), boardModelLoaded(false),
      pieceModelLight({}), pieceModelDark({}), pieceModelsLoaded(false),
      glossShader({}), glossShaderLoaded(false), glossViewPosLoc(0),
      matteShader({}), matteShaderLoaded(false), matteViewPosLoc(0),
      groundModel({}), groundLoaded(false) {}

Renderer::~Renderer() = default;

void Renderer::init(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    float center = Board::SIZE / 2.0f;
    cameraTarget = {center, -0.5f, center};

    // Default orbital parameters
    defaultAngle = 0.0f;          // looking from front
    defaultPitch = 0.85f;         // ~49 degrees down
    defaultDistance = 28.0f;

    cameraAngle = defaultAngle;
    cameraPitch = defaultPitch;
    cameraDistance = defaultDistance;

    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    rebuildCameraFromOrbit();

    // Load 3D board/table model
    if (FileExists("assets/models/table.glb")) {
        boardModel = LoadModel("assets/models/table.glb");
        boardModelLoaded = true;
        // raylib GLB loader puts default material at [0] and loaded material at [1]
        // Assign the textured material to the mesh
        if (boardModel.materialCount > 1) {
            boardModel.meshMaterial[0] = 1;
        }
    }

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

    // Menu/Settings/Restart buttons — top-right
    btnSettings = {static_cast<float>(width) - BTN_PAD - 80, BTN_PAD, 80, BTN_SIZE};
    btnMenu = {static_cast<float>(width) - BTN_PAD - 80 - BTN_PAD - 60, BTN_PAD, 60, BTN_SIZE};
    btnRestart = {static_cast<float>(width) - BTN_PAD - 80 - BTN_PAD - 60 - BTN_PAD - 70, BTN_PAD, 70, BTN_SIZE};

    // Generate procedural wood textures and piece models
    {
        const int texW = 128;
        const int texH = 128;

        // Lambda to generate a wood grain texture image
        auto generateWoodImage = [](int w, int h,
                                     Color base, Color ring, Color highlight) -> Image {
            Image img = GenImageColor(w, h, base);
            // Wood ring pattern: concentric ellipses with noise
            for (int py = 0; py < h; py++) {
                for (int px = 0; px < w; px++) {
                    float nx = (static_cast<float>(px) / static_cast<float>(w)) - 0.5f;
                    float ny = (static_cast<float>(py) / static_cast<float>(h)) - 0.5f;
                    // Distance from center with stretching for oval rings
                    float dist = sqrtf(nx * nx * 4.0f + ny * ny);
                    // Ring pattern
                    float ringVal = sinf(dist * 40.0f);
                    // Simple pseudo-noise using trig
                    float noise = sinf(static_cast<float>(px) * 0.7f + static_cast<float>(py) * 1.3f) *
                                  cosf(static_cast<float>(px) * 1.1f - static_cast<float>(py) * 0.9f);
                    ringVal += noise * 0.3f;

                    auto clamp = [](int v) -> unsigned char {
                        return static_cast<unsigned char>(v < 0 ? 0 : (v > 255 ? 255 : v));
                    };

                    Color c;
                    if (ringVal > 0.3f) {
                        // Ring line
                        float t = (ringVal - 0.3f) / 0.7f;
                        c.r = clamp(base.r + static_cast<int>(static_cast<float>(ring.r - base.r) * t));
                        c.g = clamp(base.g + static_cast<int>(static_cast<float>(ring.g - base.g) * t));
                        c.b = clamp(base.b + static_cast<int>(static_cast<float>(ring.b - base.b) * t));
                    } else {
                        // Between rings — subtle highlight variation
                        float t = (ringVal + 1.0f) / 1.3f;
                        c.r = clamp(base.r + static_cast<int>(static_cast<float>(highlight.r - base.r) * t * 0.3f));
                        c.g = clamp(base.g + static_cast<int>(static_cast<float>(highlight.g - base.g) * t * 0.3f));
                        c.b = clamp(base.b + static_cast<int>(static_cast<float>(highlight.b - base.b) * t * 0.3f));
                    }
                    c.a = 255;
                    ImageDrawPixel(&img, px, py, c);
                }
            }
            return img;
        };

        // Light maple wood (PlayerX)
        Image imgLight = generateWoodImage(texW, texH,
            {210, 180, 140, 255},   // base: warm light wood
            {180, 145, 100, 255},   // ring: darker grain lines
            {230, 205, 170, 255});  // highlight: lighter between rings
        Texture2D texLight = LoadTextureFromImage(imgLight);
        UnloadImage(imgLight);

        // Dark walnut wood (PlayerO)
        Image imgDark = generateWoodImage(texW, texH,
            {100, 65, 40, 255},     // base: dark walnut
            {70, 42, 25, 255},      // ring: darker grain
            {125, 85, 55, 255});    // highlight: lighter between rings
        Texture2D texDark = LoadTextureFromImage(imgDark);
        UnloadImage(imgDark);

        // Create sphere mesh (flattened when drawn to look like Go stones)
        Mesh stoneMesh = GenMeshSphere(0.35f, 16, 16);

        // Build light piece model
        pieceModelLight = LoadModelFromMesh(stoneMesh);
        pieceModelLight.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texLight;

        // Build dark piece model — need a separate mesh copy since LoadModelFromMesh takes ownership
        Mesh stoneMesh2 = GenMeshSphere(0.35f, 16, 16);
        pieceModelDark = LoadModelFromMesh(stoneMesh2);
        pieceModelDark.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texDark;

        pieceModelsLoaded = true;

        // Load glossy Phong shader from embedded strings
        glossShader = LoadShaderFromMemory(glossVS, glossFS);
        glossShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(glossShader, "matModel");
        glossShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(glossShader, "matNormal");
        glossViewPosLoc = GetShaderLocation(glossShader, "viewPos");
        glossShaderLoaded = true;

        // Assign the glossy shader to both piece models
        pieceModelLight.materials[0].shader = glossShader;
        pieceModelDark.materials[0].shader = glossShader;
    }

    // Load matte Phong shader for board (low specular, satin wood finish)
    matteShader = LoadShaderFromMemory(glossVS, matteFS);
    matteShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(matteShader, "matModel");
    matteShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(matteShader, "matNormal");
    matteViewPosLoc = GetShaderLocation(matteShader, "viewPos");
    matteShaderLoaded = true;

    // Assign matte shader to board model
    if (boardModelLoaded) {
        for (int i = 0; i < boardModel.materialCount; i++) {
            boardModel.materials[i].shader = matteShader;
        }
    }

    // Load ground texture and create tiled plane
    if (FileExists("assets/textures/ground.png")) {
        // Large plane, 4x4 subdivisions for tiling UV
        Mesh plane = GenMeshPlane(20.0f, 20.0f, 1, 1);
        groundModel = LoadModelFromMesh(plane);
        Texture2D groundTex = LoadTexture("assets/textures/ground.png");
        SetTextureFilter(groundTex, TEXTURE_FILTER_BILINEAR);
        SetTextureWrap(groundTex, TEXTURE_WRAP_REPEAT);
        groundModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = groundTex;
        groundLoaded = true;
    }
}

void Renderer::shutdown() {
    if (boardModelLoaded) {
        UnloadModel(boardModel);
        boardModelLoaded = false;
    }
    if (pieceModelsLoaded) {
        UnloadModel(pieceModelLight);
        UnloadModel(pieceModelDark);
        pieceModelsLoaded = false;
    }
    if (glossShaderLoaded) {
        UnloadShader(glossShader);
        glossShaderLoaded = false;
    }
    if (matteShaderLoaded) {
        UnloadShader(matteShader);
        matteShaderLoaded = false;
    }
    if (groundLoaded) {
        UnloadModel(groundModel);
        groundLoaded = false;
    }
}

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
        || CheckCollisionPointRec(point, btnLoad)
        || CheckCollisionPointRec(point, btnMenu)
        || CheckCollisionPointRec(point, btnSettings)
        || CheckCollisionPointRec(point, btnRestart);
}

bool Renderer::drawButton(Rectangle rect, const char* label, int fontSize) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    bool held = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    Color bg = held ? Color{80, 80, 80, 220} : (hovered ? Color{70, 70, 70, 200} : Color{50, 50, 50, 180});
    Color border = hovered ? WHITE : Color{150, 150, 150, 200};

    DrawRectangleRec(rect, bg);
    DrawRectangleLinesEx(rect, 1.0f, border);

    int tw = Fonts::measure(Fonts::bold, label, static_cast<float>(fontSize));
    auto tx = static_cast<int>(rect.x + (rect.width - static_cast<float>(tw)) / 2.0f);
    auto ty = static_cast<int>(rect.y + (rect.height - static_cast<float>(fontSize)) / 2.0f);
    Fonts::draw(Fonts::bold, label, tx, ty, static_cast<float>(fontSize), WHITE);

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

bool Renderer::drawMenuButton() {
    return drawButton(btnMenu, "Menu", 14);
}

bool Renderer::drawSettingsButton() {
    return drawButton(btnSettings, "Settings", 14);
}

bool Renderer::drawRestartButton() {
    return drawButton(btnRestart, "Restart", 14);
}

void Renderer::drawBoard(const Board& board, int cursorRow, int cursorCol,
                          CellState currentTurn) {
    auto now = static_cast<float>(GetTime());

    BeginMode3D(camera);

    // Update shaders with current camera position for specular reflections
    float viewPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    if (glossShaderLoaded) {
        SetShaderValue(glossShader, glossViewPosLoc, viewPos, SHADER_UNIFORM_VEC3);
    }
    if (matteShaderLoaded) {
        SetShaderValue(matteShader, matteViewPosLoc, viewPos, SHADER_UNIFORM_VEC3);
    }

    drawBoardSurface();

    // Detect new pieces and draw with animation
    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            CellState cell = board.getCell(r, c);
            if (cell != CellState::Empty && prevCells[r][c] == CellState::Empty) {
                pieceAnimStart[r][c] = now;
                lastMove = {r, c};

                // Emit placement particles
                float px = static_cast<float>(c) + 0.5f;
                float pz = static_cast<float>(r) + 0.5f;
                Color baseColor = (cell == CellState::PlayerX)
                    ? Color{50, 120, 220, 255}
                    : Color{220, 60, 60, 255};
                particles.emitPlacement(px, 0.2f, pz, baseColor);
                particles.emitLanding(px, pz, baseColor);
            }
            prevCells[r][c] = cell;

            if (cell != CellState::Empty) {
                float anim = 1.0f;
                if (pieceAnimStart[r][c] > 0.0f) {
                    anim = (now - pieceAnimStart[r][c]) / PIECE_ANIM_DURATION;
                    if (anim > 1.0f) anim = 1.0f;
                    if (anim < 0.0f) anim = 0.0f;
                }
                drawPiece3D(r, c, cell, anim);
            }
        }
    }

    // Last move indicator
    if (lastMove.row >= 0 && lastMove.col >= 0) {
        drawLastMoveIndicator(lastMove.row, lastMove.col, now);
    }

    if (hoverValid) {
        drawCursor3D(hoverRow, hoverCol, currentTurn);
    } else {
        drawCursor3D(cursorRow, cursorCol, currentTurn);
    }

    // Draw particles in 3D space
    particles.draw();

    EndMode3D();
}

void Renderer::drawWinLine(const std::vector<Move>& winLine) {
    auto now = static_cast<float>(GetTime());

    if (!showingWinLine) {
        winLineStart = now;
        showingWinLine = true;
    }

    // Emit win celebration particles once
    if (!winParticlesEmitted) {
        winParticlesEmitted = true;
        std::vector<std::pair<float, float>> positions;
        for (const auto& m : winLine) {
            positions.push_back({
                static_cast<float>(m.col) + 0.5f,
                static_cast<float>(m.row) + 0.5f
            });
        }
        particles.emitWinCelebration(positions);
    }

    BeginMode3D(camera);

    for (int i = 0; i < static_cast<int>(winLine.size()); i++) {
        const auto& m = winLine[static_cast<size_t>(i)];
        float x = static_cast<float>(m.col) + 0.5f;
        float z = static_cast<float>(m.row) + 0.5f;

        // Sequential delay: each cell lights up 0.1s after the previous
        float cellStart = winLineStart + static_cast<float>(i) * 0.1f;
        if (now < cellStart) continue;

        // Pulsing alpha and height
        float pulse = std::sin(now * WIN_PULSE_SPEED);
        float alpha = 0.5f + 0.3f * pulse;
        float height = 0.04f + 0.04f * (0.5f + 0.5f * pulse);
        float y = height * 0.5f;

        DrawCube({x, y, z}, 0.95f, height, 0.95f, Fade(GOLD, alpha));
        DrawCubeWires({x, y, z}, 0.95f, height, 0.95f, GOLD);
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

    // Tiled textured ground — 7x7 grid of 20x20 tiles centered on board
    if (groundLoaded) {
        float tileSize = 20.0f;
        for (int tx = -3; tx <= 3; tx++) {
            for (int tz = -3; tz <= 3; tz++) {
                float gx = half + static_cast<float>(tx) * tileSize;
                float gz = half + static_cast<float>(tz) * tileSize;
                DrawModel(groundModel, {gx, -0.8f, gz}, 1.0f, WHITE);
            }
        }
    }

    // Soft shadow under the board
    float shadowSize = static_cast<float>(Board::SIZE) + 3.0f;
    DrawPlane({half, -0.79f, half}, {shadowSize, shadowSize}, {0, 0, 0, 60});

    if (boardModelLoaded) {
        // Go board model: ~35 units wide, 19x19 grid baked into wood texture
        static constexpr float MODEL_GRID_EXTENT = 31.5f;
        static constexpr float MODEL_TOP_Y = 1.1f;

        float worldGridExtent = static_cast<float>(Board::SIZE - 1);  // 18.0
        float scale = worldGridExtent / MODEL_GRID_EXTENT;
        float worldCenter = Board::SIZE / 2.0f;  // 9.5
        float yOffset = -MODEL_TOP_Y * scale;

        // Board uses default shader — no glossy reflection, just natural wood
        DrawModelEx(boardModel,
                    {worldCenter, yOffset, worldCenter},
                    {0.0f, 1.0f, 0.0f}, 0.0f,
                    {scale, scale, scale},
                    WHITE);
    } else {
        // Fallback: plain wooden box if model not loaded
        float half = Board::SIZE / 2.0f;
        float boardLen = static_cast<float>(Board::SIZE);
        float padding = 0.5f;
        float boardHeight = 0.3f;
        DrawCube({half, -boardHeight / 2.0f, half},
                 boardLen + padding * 2, boardHeight, boardLen + padding * 2,
                 {220, 179, 92, 255});
        DrawCubeWires({half, -boardHeight / 2.0f, half},
                      boardLen + padding * 2, boardHeight, boardLen + padding * 2,
                      {180, 140, 60, 255});
    }
}

void Renderer::drawVignette() {
    // Darken screen edges with semi-transparent black rectangles (gradient approximation)
    int w = screenWidth;
    int h = screenHeight;
    int bands = 8;
    int edgeSize = w / 4;  // vignette covers outer 25% of each edge

    for (int i = 0; i < bands; i++) {
        float t = static_cast<float>(i) / static_cast<float>(bands);
        auto alpha = static_cast<unsigned char>(40.0f * (1.0f - t));  // fade from 40 to 0
        Color c = {0, 0, 0, alpha};
        int bandW = static_cast<int>(static_cast<float>(edgeSize) * (1.0f - t));

        // Left edge
        DrawRectangle(0, 0, bandW / bands, h, c);
        // Right edge
        DrawRectangle(w - bandW / bands, 0, bandW / bands, h, c);
        // Top edge
        DrawRectangle(0, 0, w, bandW / bands, c);
        // Bottom edge
        DrawRectangle(0, h - bandW / bands, w, bandW / bands, c);
    }
}

void Renderer::drawPiece3D(int row, int col, CellState state, float anim) {
    float x = static_cast<float>(col) + 0.5f;
    float z = static_cast<float>(row) + 0.5f;

    float t = (anim < 1.0f) ? anim : 1.0f;  // 0..1 over PIECE_ANIM_DURATION

    // --- Smooth drop with one gentle settle ---
    static constexpr float DROP_HEIGHT = 3.0f;
    static constexpr float SETTLE_HEIGHT = 0.15f;
    static constexpr float PHASE1_END = 0.6f;

    float y;
    float squashX = 1.0f;
    float squashY = 1.0f;

    if (t < PHASE1_END) {
        float p = t / PHASE1_END;
        float eased = 1.0f - (1.0f - p) * (1.0f - p) * (1.0f - p);
        y = DROP_HEIGHT * (1.0f - eased);

        if (y < 0.2f) {
            float proximity = 1.0f - (y / 0.2f);
            float squash = 0.15f * proximity;
            squashY = 1.0f - squash;
            squashX = 1.0f + squash * 0.5f;
        }
    } else {
        float p = (t - PHASE1_END) / (1.0f - PHASE1_END);
        y = SETTLE_HEIGHT * 4.0f * p * (1.0f - p) * 0.7f;
        if (p > 0.7f) {
            float tail = (p - 0.7f) / 0.3f;
            float squash = 0.05f * tail;
            squashY = 1.0f - squash;
            squashX = 1.0f + squash * 0.3f;
        }
    }

    // Scale: fade in during first 20% of animation
    float scaleFactor;
    if (t < 0.2f) {
        float st = t / 0.2f;
        scaleFactor = 0.5f + 0.5f * st;
    } else {
        scaleFactor = 1.0f;
    }

    // --- Drop shadow: filled dark disc on board surface ---
    {
        float shadowAlpha;
        float shadowRadius;
        if (t < 1.0f) {
            float heightRatio = y / DROP_HEIGHT;
            shadowAlpha = 0.35f * (1.0f - heightRatio);
            shadowRadius = 0.2f + 0.15f * (1.0f - heightRatio);
        } else {
            shadowAlpha = 0.2f;
            shadowRadius = 0.35f;
        }
        auto alpha = static_cast<unsigned char>(shadowAlpha * 255.0f);
        // DrawCylinder with zero height = filled disc
        DrawCylinder({x, 0.01f, z}, shadowRadius, shadowRadius, 0.001f, 16,
                     {0, 0, 0, alpha});
    }

    // --- Draw the piece ---
    float s = scaleFactor;
    if (pieceModelsLoaded) {
        Vector3 pos = {x, y + 0.15f * s, z};
        Vector3 scaleVec = {s * squashX, s * 0.5f * squashY, s * squashX};
        Model& model = (state == CellState::PlayerX) ? pieceModelLight : pieceModelDark;
        DrawModelEx(model, pos, {0.0f, 1.0f, 0.0f}, 0.0f, scaleVec, WHITE);
    } else {
        if (state == CellState::PlayerX) {
            float radius = 0.3f * s * squashX;
            float height = 0.4f * s * squashY;
            DrawCylinder({x, y, z}, 0.0f, radius, height, 8, {50, 120, 220, 255});
            DrawCylinderWires({x, y, z}, 0.0f, radius, height, 8, {30, 80, 180, 255});
        } else if (state == CellState::PlayerO) {
            float radius = 0.35f * s * squashX;
            float height = 0.2f * s * squashY;
            DrawCylinder({x, y, z}, radius, radius, height, 16, {220, 60, 60, 255});
            DrawCylinderWires({x, y, z}, radius, radius, height, 16, {180, 30, 30, 255});
        }
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

void Renderer::drawLastMoveIndicator(int row, int col, float time) {
    float x = static_cast<float>(col) + 0.5f;
    float z = static_cast<float>(row) + 0.5f;

    // Pulsing ring around the last placed piece
    float pulse = std::sin(time * LAST_MOVE_PULSE_SPEED);
    float alpha = 0.3f + 0.2f * pulse;
    float radius = 0.4f + 0.05f * pulse;

    DrawCircle3D({x, 0.01f, z}, radius, {1.0f, 0.0f, 0.0f}, 90.0f,
                 Fade(WHITE, alpha));
}

void Renderer::resetAnimations() {
    for (int r = 0; r < Board::SIZE; r++) {
        for (int c = 0; c < Board::SIZE; c++) {
            prevCells[r][c] = CellState::Empty;
            pieceAnimStart[r][c] = 0.0f;
        }
    }
    lastMove = {-1, -1};
    winLineStart = 0.0f;
    showingWinLine = false;
    winParticlesEmitted = false;
    particles.clear();
}

void Renderer::updateParticles(float dt) {
    particles.update(dt);
}

void Renderer::triggerWinParticles(const std::vector<Move>& winLine) {
    std::vector<std::pair<float, float>> positions;
    for (const auto& m : winLine) {
        positions.push_back({
            static_cast<float>(m.col) + 0.5f,
            static_cast<float>(m.row) + 0.5f
        });
    }
    particles.emitWinCelebration(positions);
}
