#include "Renderer.h"
#include "Fonts.h"
#include "Theme.h"
#include "rlgl.h"
#include <cmath>

// Backdrop tunables — adjust freely without touching position math elsewhere.
// kBackdropWidth fit-scales the model so it spans this many world units in
// X (or Z, whichever is larger). kBackdropY is the world-Y for the model's
// origin; raise this to sink the board into the mountains, lower to make
// the diorama look small/distant.
static constexpr float kBackdropWidth = 300.0f;
static constexpr float kBackdropY     = -40.0f;

// Floating-island pedestal under the board. kIslandWidth fit-scales the model
// so it spans this many world units in X (or Z) — wider than the 21-unit
// board diagonal so the corners don't overhang. kIslandTopY is the target
// world-Y for the *top* of the scaled island; the load-time math places the
// model so its bbox top lands here regardless of where the model's local
// origin sits inside its mesh. Tune this single number to lift/drop the rock.
static constexpr float kIslandWidth = 28.0f;
static constexpr float kIslandTopY  = -0.5f;

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
    "    float ambientStrength = 0.30;\n"
    "    float specularStrength = 1.0;\n"
    "    float shininess = 96.0;\n"
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

// Edge-fade vertex shader for the scroll backdrop. Passes world-space
// fragPosition + vertex color through; raylib auto-fills mvp + matModel.
static const char* edgeFadeVS =
    "#version 330\n"
    "in vec3 vertexPosition;\n"
    "in vec2 vertexTexCoord;\n"
    "in vec4 vertexColor;\n"
    "uniform mat4 mvp;\n"
    "uniform mat4 matModel;\n"
    "out vec3 fragPosition;\n"
    "out vec2 fragTexCoord;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));\n"
    "    fragTexCoord = vertexTexCoord;\n"
    "    fragColor = vertexColor;\n"
    "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
    "}\n";

// Edge-fade fragment shader: multiply texture alpha by ellipse-distance
// falloff from scroll center in XZ. Inside fadeStart radius -> fully opaque;
// between fadeStart and 1.0 -> smoothstep falloff to transparent. Gradient
// sky behind the scroll shows through the faded edges.
static const char* edgeFadeFS =
    "#version 330\n"
    "in vec3 fragPosition;\n"
    "in vec2 fragTexCoord;\n"
    "in vec4 fragColor;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D texture0;\n"
    "uniform vec4 colDiffuse;\n"
    "uniform vec3 scrollCenter;\n"
    "uniform float scrollHalfX;\n"
    "uniform float scrollHalfZ;\n"
    "uniform float fadeStart;\n"
    "void main() {\n"
    "    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse * fragColor;\n"
    "    float nx = (fragPosition.x - scrollCenter.x) / scrollHalfX;\n"
    "    float nz = (fragPosition.z - scrollCenter.z) / scrollHalfZ;\n"
    "    float distRatio = sqrt(nx * nx + nz * nz);\n"
    "    float alphaMult = 1.0 - smoothstep(fadeStart, 1.0, distRatio);\n"
    "    finalColor = vec4(texColor.rgb, texColor.a * alphaMult);\n"
    "}\n";

// Vignette fragment shader — radial smoothstep on screen-space distance
// from center, multiplied into a black overlay alpha. Drawn over the 3D
// scene with a fullscreen rectangle: rectangle's fragTexCoord spans 0..1.
// Max distance from center to corner is sqrt(0.5) ≈ 0.707, so outer radius
// must stay below that for corners to actually hit full darken.
static const char* vignetteFS =
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "out vec4 finalColor;\n"
    "void main() {\n"
    "    vec2 d = fragTexCoord - vec2(0.5);\n"
    "    float r = length(d);\n"
    "    float a = smoothstep(0.20, 0.60, r) * 0.85;\n"
    "    finalColor = vec4(0.0, 0.0, 0.0, a);\n"
    "}\n";

// Sky-gradient fragment shader — 3-stop smoothstep between
// sky_top / sky_mid / sky_horizon (fed as vec3 uniforms each frame).
// Vertex shader uses raylib's default (passes fragTexCoord through).
static const char* skyFS =
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "out vec4 finalColor;\n"
    "uniform vec3 colorTop;\n"
    "uniform vec3 colorMid;\n"
    "uniform vec3 colorBot;\n"
    "void main() {\n"
    "    float t = fragTexCoord.y;\n"
    "    vec3 col = (t < 0.5)\n"
    "        ? mix(colorTop, colorMid, smoothstep(0.0, 0.5, t))\n"
    "        : mix(colorMid, colorBot, smoothstep(0.5, 1.0, t));\n"
    "    finalColor = vec4(col, 1.0);\n"
    "}\n";

// Matte wood shader for the board — low specular, soft diffuse
// Board sits on XZ plane at Y=0, cells [0..SIZE) on both axes
// Each cell is 1.0 world unit

static const float ORBIT_SPEED = 0.005f;     // radians per pixel of drag
static const float BUTTON_ORBIT_SPEED = 0.05f; // radians per frame when button held
static const float ZOOM_SPEED = 0.5f;
static const float MIN_DISTANCE = 8.0f;
static const float MAX_DISTANCE = 80.0f;
static const float MIN_PITCH = 11.0f * DEG2RAD;  // don't go below board
static const float MAX_PITCH = 80.0f * DEG2RAD;  // nearly top-down
static const int BTN_SIZE = 36;
static const int BTN_PAD = 6;

// Loads a GLB and computes a uniform XZ-fit scale + bbox-recenter offset so
// the model spans `widthXZ` world units across its larger horizontal axis and
// its bbox center sits at the caller's draw position. Caller can override
// outOffset.y afterwards (e.g., to anchor the bbox top at a specific world Y).
// Returns false if the file is missing or the model has no meshes.
static bool loadFittedXZ(const char* path, float widthXZ, Model& outModel,
                         float& outScale, Vector3& outOffset) {
    if (!FileExists(path)) return false;
    Model m = LoadModel(path);
    if (m.meshCount == 0) {
        UnloadModel(m);
        return false;
    }
    BoundingBox bb = GetModelBoundingBox(m);
    float dx = bb.max.x - bb.min.x;
    float dz = bb.max.z - bb.min.z;
    float dmax = dx > dz ? dx : dz;
    outScale = (dmax > 0.0f) ? (widthXZ / dmax) : 1.0f;
    outOffset.x = -((bb.min.x + bb.max.x) * 0.5f) * outScale;
    outOffset.y = 0.0f;
    outOffset.z = -((bb.min.z + bb.max.z) * 0.5f) * outScale;
    outModel = m;
    return true;
}

Renderer::Renderer()
    : camera({}), cellSize(1.0f), screenWidth(0), screenHeight(0),
      cameraAngle(0), cameraPitch(0), cameraDistance(0),
      cameraTarget({}), defaultAngle(0), defaultPitch(0), defaultDistance(0),
      isDragging(false), dragStart({}),
      btnRotateLeft({}), btnRotateRight({}), btnZoomIn({}), btnZoomOut({}), btnReset({}),
      btnSave({}), btnLoad({}), btnMenu({}), btnSettings({}), btnRestart({}), btnUndo({}),
      hoverValid(false), hoverRow(0), hoverCol(0),
      prevCells{}, pieceAnimStart{}, lastMove{-1, -1},
      winLineStart(0.0f), showingWinLine(false),
      winParticlesEmitted(false),
      boardModel({}), boardModelLoaded(false),
      pieceModelLight({}), pieceModelDark({}), pieceModelsLoaded(false),
      pieceModelSon({}), pieceModelThuy({}),
      pieceScaleSon(1.0f), pieceScaleThuy(1.0f), pieceGLBLoaded(false),
      glossShader({}), glossShaderLoaded(false), glossViewPosLoc(0),
      skyShader({}), skyShaderLoaded(false),
      skyTopLoc(0), skyMidLoc(0), skyBotLoc(0),
      edgeFadeShader({}), edgeFadeShaderLoaded(false),
      vignetteShader({}), vignetteShaderLoaded(false),
      scrollModel({}), scrollLoaded(false), scrollScale(1.0f),
      scrollCenterOffset({0.0f, 0.0f, 0.0f}),
      islandModel({}), islandLoaded(false), islandScale(1.0f),
      islandCenterOffset({0.0f, 0.0f, 0.0f}) {}

Renderer::~Renderer() = default;

void Renderer::init(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    float center = Board::SIZE / 2.0f;
    cameraTarget = {center, -0.5f, center};

    // Default orbital parameters
    defaultAngle = 0.0f;          // looking from front
    defaultPitch = 40.0f * DEG2RAD;
    defaultDistance = 35.0f;

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
    btnUndo = {btnRestart.x - BTN_PAD - 60, BTN_PAD, 60, BTN_SIZE};

    // Try GLB runes first; fall back to sphere stones with per-cell PNG
    // textures only when the runes can't load. Wrapping the fallback path in
    // a single `if` skips a 450-PNG disk scan + ~450 unused 1024² uploads
    // when the runes are present.
    {
        auto fitScale = [](const Model& m) -> float {
            constexpr float kCellFit = 0.7f;
            BoundingBox bb = GetModelBoundingBox(m);
            float dx = bb.max.x - bb.min.x;
            float dy = bb.max.y - bb.min.y;
            float dz = bb.max.z - bb.min.z;
            float dmax = dx > dy ? dx : dy;
            if (dz > dmax) dmax = dz;
            return dmax > 0.0f ? (kCellFit / dmax) : 1.0f;
        };
        pieceModelSon  = LoadModel("assets/models/fire_rune.glb");
        pieceModelThuy = LoadModel("assets/models/water_rune.glb");
        if (pieceModelSon.meshCount > 0 && pieceModelThuy.meshCount > 0) {
            // GLB-loader quirk seen on table.glb: when there are 2 materials,
            // the textured one lives at index 1 — point the mesh at it.
            if (pieceModelSon.materialCount > 1)  pieceModelSon.meshMaterial[0]  = 1;
            if (pieceModelThuy.materialCount > 1) pieceModelThuy.meshMaterial[0] = 1;
            pieceScaleSon  = fitScale(pieceModelSon);
            pieceScaleThuy = fitScale(pieceModelThuy);
            pieceGLBLoaded = true;
        } else {
            UnloadModel(pieceModelSon);
            UnloadModel(pieceModelThuy);
            pieceModelSon  = {};
            pieceModelThuy = {};
        }
    }

    if (!pieceGLBLoaded) {
        Image defLight = GenImageColor(4, 4, {210, 180, 140, 255});
        defaultTexLight = LoadTextureFromImage(defLight);
        UnloadImage(defLight);
        Image defDark = GenImageColor(4, 4, {100, 65, 40, 255});
        defaultTexDark = LoadTextureFromImage(defDark);
        UnloadImage(defDark);

        for (int r = 0; r < Board::SIZE; r++) {
            for (int c = 0; c < Board::SIZE; c++) {
                pieceTexLight[r][c] = defaultTexLight;
                pieceTexDark[r][c] = defaultTexDark;
                pieceTexReady[r][c] = false;
            }
        }

        imagesLoaded.store(0);
        texUploadIndex = 0;
        texLoaderThread = std::thread([this]() {
            char path[128];
            for (int r = 0; r < Board::SIZE; r++) {
                for (int c = 0; c < Board::SIZE; c++) {
                    std::snprintf(path, sizeof(path), "assets/pieces/light_%d_%d.png", r, c);
                    pieceImgLight[r][c] = LoadImage(path);
                    std::snprintf(path, sizeof(path), "assets/pieces/dark_%d_%d.png", r, c);
                    pieceImgDark[r][c] = LoadImage(path);
                    imagesLoaded.fetch_add(1);
                }
            }
        });

        Mesh stoneMesh = GenMeshSphere(0.35f, 16, 16);
        pieceModelLight = LoadModelFromMesh(stoneMesh);
        pieceModelLight.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = defaultTexLight;

        Mesh stoneMesh2 = GenMeshSphere(0.35f, 16, 16);
        pieceModelDark = LoadModelFromMesh(stoneMesh2);
        pieceModelDark.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = defaultTexDark;

        pieceModelsLoaded = true;
    }

    glossShader = LoadShaderFromMemory(glossVS, glossFS);
    glossShader.locs[SHADER_LOC_MATRIX_MODEL]  = GetShaderLocation(glossShader, "matModel");
    glossShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(glossShader, "matNormal");
    glossViewPosLoc = GetShaderLocation(glossShader, "viewPos");
    glossShaderLoaded = true;

    // Sky-gradient: NULL vertex source -> raylib's default vert shader.
    skyShader = LoadShaderFromMemory(nullptr, skyFS);
    skyTopLoc = GetShaderLocation(skyShader, "colorTop");
    skyMidLoc = GetShaderLocation(skyShader, "colorMid");
    skyBotLoc = GetShaderLocation(skyShader, "colorBot");
    skyShaderLoaded = true;

    edgeFadeShader = LoadShaderFromMemory(edgeFadeVS, edgeFadeFS);
    edgeFadeShader.locs[SHADER_LOC_MATRIX_MODEL] =
        GetShaderLocation(edgeFadeShader, "matModel");
    edgeFadeShaderLoaded = true;

    vignetteShader = LoadShaderFromMemory(nullptr, vignetteFS);
    vignetteShaderLoaded = true;

    if (pieceModelsLoaded) {
        pieceModelLight.materials[0].shader = glossShader;
        pieceModelDark.materials[0].shader  = glossShader;
    }
    if (pieceGLBLoaded) {
        // raylib's default shader renders GLB textures flat — gloss adds the
        // ambient + directional shading the carved rune detail needs.
        for (int i = 0; i < pieceModelSon.materialCount; i++)
            pieceModelSon.materials[i].shader = glossShader;
        for (int i = 0; i < pieceModelThuy.materialCount; i++)
            pieceModelThuy.materials[i].shader = glossShader;
    }

    if (boardModelLoaded) {
        for (int i = 0; i < boardModel.materialCount; i++) {
            boardModel.materials[i].shader = glossShader;
        }
    }

    // Backdrop scroll — leave its materials alone; KHR_materials_unlit + vertex
    // colors already carry the look, and forcing gloss would re-light a model
    // that's authored as flat-shaded anime.
    scrollLoaded = loadFittedXZ("assets/models/mountain_and_river_scroll.glb",
                                kBackdropWidth, scrollModel, scrollScale,
                                scrollCenterOffset);
    if (scrollLoaded) {
        // Effective bbox = union of meshes whose XZ-footprint is >= 5% of
        // the largest mesh's footprint. Skips thin decorative outliers that
        // would otherwise drag the fade ellipse off the painted content.
        std::vector<BoundingBox> bboxes(scrollModel.meshCount);
        std::vector<float> areas(scrollModel.meshCount);
        float maxArea = 0.0f;
        for (int i = 0; i < scrollModel.meshCount; i++) {
            bboxes[i] = GetMeshBoundingBox(scrollModel.meshes[i]);
            areas[i] = (bboxes[i].max.x - bboxes[i].min.x) *
                       (bboxes[i].max.z - bboxes[i].min.z);
            if (areas[i] > maxArea) maxArea = areas[i];
        }
        const float threshold = maxArea * 0.05f;
        BoundingBox effective = {{0,0,0}, {0,0,0}};
        bool seeded = false;
        for (int i = 0; i < scrollModel.meshCount; i++) {
            if (areas[i] < threshold) continue;
            if (!seeded) {
                effective = bboxes[i];
                seeded = true;
            } else {
                if (bboxes[i].min.x < effective.min.x) effective.min.x = bboxes[i].min.x;
                if (bboxes[i].max.x > effective.max.x) effective.max.x = bboxes[i].max.x;
                if (bboxes[i].min.z < effective.min.z) effective.min.z = bboxes[i].min.z;
                if (bboxes[i].max.z > effective.max.z) effective.max.z = bboxes[i].max.z;
            }
        }
        float scrollHalfX = (effective.max.x - effective.min.x) * scrollScale * 0.5f;
        float scrollHalfZ = (effective.max.z - effective.min.z) * scrollScale * 0.5f;
        float fadeOffsetX = (effective.min.x + effective.max.x) * 0.5f * scrollScale;
        float fadeOffsetZ = (effective.min.z + effective.max.z) * 0.5f * scrollScale;
        if (edgeFadeShaderLoaded) {
            for (int i = 0; i < scrollModel.materialCount; i++) {
                scrollModel.materials[i].shader = edgeFadeShader;
            }
            // Scroll position + extents are static post-init — push uniforms
            // once. fadeStart=0.4 = inner 40% opaque, outer 60% smoothstep.
            const float half = Board::SIZE / 2.0f;
            float scrollCenter[3] = {
                half + scrollCenterOffset.x + fadeOffsetX,
                kBackdropY,
                half + scrollCenterOffset.z + fadeOffsetZ,
            };
            float fadeStart = 0.4f;
            int centerLoc = GetShaderLocation(edgeFadeShader, "scrollCenter");
            int halfXLoc  = GetShaderLocation(edgeFadeShader, "scrollHalfX");
            int halfZLoc  = GetShaderLocation(edgeFadeShader, "scrollHalfZ");
            int fadeLoc   = GetShaderLocation(edgeFadeShader, "fadeStart");
            SetShaderValue(edgeFadeShader, centerLoc, scrollCenter, SHADER_UNIFORM_VEC3);
            SetShaderValue(edgeFadeShader, halfXLoc,  &scrollHalfX,  SHADER_UNIFORM_FLOAT);
            SetShaderValue(edgeFadeShader, halfZLoc,  &scrollHalfZ,  SHADER_UNIFORM_FLOAT);
            SetShaderValue(edgeFadeShader, fadeLoc,   &fadeStart,    SHADER_UNIFORM_FLOAT);
        }
    }

    // Floating-island pedestal. The asset is a ZBrush sculpt with no textures;
    // raylib's default shader is unlit, so without overriding it the rock
    // would render as a flat grey silhouette regardless of normals. Gloss
    // gives diffuse + specular off the sculpt's normals.
    if (loadFittedXZ("assets/models/floating_island.glb", kIslandWidth,
                     islandModel, islandScale, islandCenterOffset)) {
        BoundingBox bb = GetModelBoundingBox(islandModel);
        islandCenterOffset.y = kIslandTopY - bb.max.y * islandScale;
        if (glossShaderLoaded) {
            for (int i = 0; i < islandModel.materialCount; i++) {
                islandModel.materials[i].shader = glossShader;
            }
        }
        islandLoaded = true;
    }
}

void Renderer::uploadPendingTextures() {
    int loaded = imagesLoaded.load();
    int total = Board::SIZE * Board::SIZE;
    // Upload up to 10 textures per frame to avoid stutter
    for (int i = 0; i < 10 && texUploadIndex < loaded && texUploadIndex < total; i++, texUploadIndex++) {
        int r = texUploadIndex / Board::SIZE;
        int c = texUploadIndex % Board::SIZE;
        pieceTexLight[r][c] = LoadTextureFromImage(pieceImgLight[r][c]);
        UnloadImage(pieceImgLight[r][c]);
        pieceTexDark[r][c] = LoadTextureFromImage(pieceImgDark[r][c]);
        UnloadImage(pieceImgDark[r][c]);
        pieceTexReady[r][c] = true;
    }
}

void Renderer::shutdown() {
    // Wait for texture loader thread
    if (texLoaderThread.joinable()) texLoaderThread.join();

    if (boardModelLoaded) {
        UnloadModel(boardModel);
        boardModelLoaded = false;
    }
    if (pieceModelsLoaded) {
        UnloadModel(pieceModelLight);
        UnloadModel(pieceModelDark);
        pieceModelsLoaded = false;
    }
    if (pieceGLBLoaded) {
        UnloadModel(pieceModelSon);
        UnloadModel(pieceModelThuy);
        pieceGLBLoaded = false;
    }
    if (glossShaderLoaded) {
        UnloadShader(glossShader);
        glossShaderLoaded = false;
    }
    if (skyShaderLoaded) {
        UnloadShader(skyShader);
        skyShaderLoaded = false;
    }
    if (edgeFadeShaderLoaded) {
        UnloadShader(edgeFadeShader);
        edgeFadeShaderLoaded = false;
    }
    if (vignetteShaderLoaded) {
        UnloadShader(vignetteShader);
        vignetteShaderLoaded = false;
    }
    if (scrollLoaded) {
        UnloadModel(scrollModel);
        scrollLoaded = false;
    }
    if (islandLoaded) {
        UnloadModel(islandModel);
        islandLoaded = false;
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

bool Renderer::updateCamera() {
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
    bool clickedUI = false;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        // Held buttons (rotate/zoom) get the click SFX on first press only;
        // continuous-while-held would spam at frame rate.
        if (isPointOnCameraButton(mouse)) {
            clickedUI = true;
        }
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

    return clickedUI;
}

bool Renderer::isPointOnCameraButton(Vector2 point) const {
    return CheckCollisionPointRec(point, btnRotateLeft)
        || CheckCollisionPointRec(point, btnRotateRight)
        || CheckCollisionPointRec(point, btnZoomIn)
        || CheckCollisionPointRec(point, btnZoomOut)
        || CheckCollisionPointRec(point, btnReset);
}

bool Renderer::isPointOnUI(Vector2 point) const {
    return isPointOnCameraButton(point)
        || CheckCollisionPointRec(point, btnSave)
        || CheckCollisionPointRec(point, btnLoad)
        || CheckCollisionPointRec(point, btnMenu)
        || CheckCollisionPointRec(point, btnSettings)
        || CheckCollisionPointRec(point, btnRestart)
        || CheckCollisionPointRec(point, btnUndo);
}

bool Renderer::drawButton(Rectangle rect, const char* label, int fontSize) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    bool held = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    Color frameColor = hovered ? Theme::palette.gold_foil : Theme::palette.ink_sumi;
    Color bodyColor  = held    ? Theme::palette.ink_sumi   : Theme::withAlpha(Theme::palette.slate_fog, 220);
    Color textColor  = hovered ? Theme::palette.gold_foil : Theme::palette.son_bone;

    const float ox = held ? 1.5f : 0.0f;
    const float oy = held ? 1.5f : 0.0f;
    Rectangle outer = { rect.x + ox, rect.y + oy, rect.width, rect.height };
    DrawRectangleRec(outer, frameColor);

    const float frameInset = 2.0f;
    Rectangle inner = {
        outer.x + frameInset, outer.y + frameInset,
        outer.width  - 2.0f * frameInset,
        outer.height - 2.0f * frameInset
    };
    DrawRectangleRec(inner, bodyColor);

    const int notchSize = hovered ? 5 : 4;
    Color notchColor = hovered
        ? Theme::palette.gold_foil
        : Theme::withAlpha(Theme::palette.gold_foil, 140);
    const int ix = static_cast<int>(inner.x);
    const int iy = static_cast<int>(inner.y);
    const int iw = static_cast<int>(inner.width);
    const int ih = static_cast<int>(inner.height);
    DrawRectangle(ix,                  iy,                  notchSize, 1,         notchColor);
    DrawRectangle(ix,                  iy,                  1,         notchSize, notchColor);
    DrawRectangle(ix + iw - notchSize, iy + ih - 1,         notchSize, 1,         notchColor);
    DrawRectangle(ix + iw - 1,         iy + ih - notchSize, 1,         notchSize, notchColor);

    int tw = Fonts::measure(Fonts::bold, label, static_cast<float>(fontSize));
    auto tx = static_cast<int>(outer.x + (outer.width - static_cast<float>(tw)) / 2.0f);
    auto ty = static_cast<int>(outer.y + (outer.height - static_cast<float>(fontSize)) / 2.0f);
    Fonts::draw(Fonts::bold, label, tx, ty, static_cast<float>(fontSize), textColor);

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

bool Renderer::drawUndoButton() {
    return drawButton(btnUndo, "Undo", 14);
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

    // 3D backdrop scroll — board reads as floating among the mountains, not
    // hovering above a toy diorama. X/Z auto-center via scrollCenterOffset
    // (computed at load); only kBackdropWidth and kBackdropY are tunables.
    // Backface culling stays on: the model has anime back-face-expanded
    // outline meshes which only read correctly from near-horizontal angles —
    // at any downward pitch they fan out as long black streaks. Trading the
    // outline effect for a clean silhouette is the right call here.
    if (scrollLoaded) {
        Vector3 scrollPos = {
            half + scrollCenterOffset.x,
            kBackdropY,
            half + scrollCenterOffset.z,
        };
        DrawModelEx(scrollModel, scrollPos, {0.0f, 1.0f, 0.0f}, 0.0f,
                    {scrollScale, scrollScale, scrollScale}, WHITE);
    }

    // Floating island pedestal — drawn after scroll, before the table, so the
    // board reads as resting on a chunk of stone hovering above the diorama.
    // islandCenterOffset.y already encodes "place bbox top at kIslandTopY" —
    // no kIslandY math here, just use the cached value.
    if (islandLoaded) {
        Vector3 islandPos = {
            half + islandCenterOffset.x,
            islandCenterOffset.y,
            half + islandCenterOffset.z,
        };
        DrawModelEx(islandModel, islandPos, {0.0f, 1.0f, 0.0f}, 0.0f,
                    {islandScale, islandScale, islandScale}, WHITE);
    }

    if (boardModelLoaded) {
        // Go board model: ~35 units wide, 19x19 grid baked into wood texture
        static constexpr float MODEL_GRID_EXTENT = 31.5f;
        static constexpr float MODEL_TOP_Y = 1.1f;

        float worldGridExtent = static_cast<float>(Board::SIZE - 1);  // 14.0
        float scale = worldGridExtent / MODEL_GRID_EXTENT;
        float worldCenter = Board::SIZE / 2.0f;  // 7.5
        float yOffset = -MODEL_TOP_Y * scale;

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

void Renderer::drawSkyGradient() {
    if (!skyShaderLoaded) {
        // Fallback: flat horizon if shader didn't load.
        ClearBackground(Theme::palette.sky_horizon);
        return;
    }

    auto toF = [](Color c, float out[3]) {
        out[0] = static_cast<float>(c.r) / 255.0f;
        out[1] = static_cast<float>(c.g) / 255.0f;
        out[2] = static_cast<float>(c.b) / 255.0f;
    };
    float topF[3]; toF(Theme::palette.sky_top,     topF);
    float midF[3]; toF(Theme::palette.sky_mid,     midF);
    float botF[3]; toF(Theme::palette.sky_horizon, botF);

    SetShaderValue(skyShader, skyTopLoc, topF, SHADER_UNIFORM_VEC3);
    SetShaderValue(skyShader, skyMidLoc, midF, SHADER_UNIFORM_VEC3);
    SetShaderValue(skyShader, skyBotLoc, botF, SHADER_UNIFORM_VEC3);

    BeginShaderMode(skyShader);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
    EndShaderMode();
}

void Renderer::drawVignette() {
    if (!vignetteShaderLoaded) return;
    BeginShaderMode(vignetteShader);
    DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
    EndShaderMode();
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
    if (pieceGLBLoaded) {
        // GLB runes are authored upright; the sphere fallback's Y-squash
        // would deform them, so animate scale uniformly here.
        float modelScale = (state == CellState::PlayerX)
                               ? pieceScaleSon : pieceScaleThuy;
        Vector3 pos = {x, y + 0.05f * s, z};
        Vector3 scaleVec = {
            s * modelScale * squashX,
            s * modelScale * squashY,
            s * modelScale * squashX
        };
        Model& model = (state == CellState::PlayerX)
                           ? pieceModelSon : pieceModelThuy;
        // Co-prime multipliers spread yaw evenly across the 15×15 board;
        // deterministic on (row, col) so save/load and replays match.
        constexpr int kYawSeedRow  = 73;
        constexpr int kYawSeedCol  = 113;
        constexpr int kYawSeedBias = 17;
        float yaw = static_cast<float>(
            (row * kYawSeedRow + col * kYawSeedCol + kYawSeedBias) % 360);
        DrawModelEx(model, pos, {0.0f, 1.0f, 0.0f}, yaw, scaleVec, WHITE);
    } else if (pieceModelsLoaded) {
        Vector3 pos = {x, y + 0.15f * s, z};
        Vector3 scaleVec = {s * squashX, s * 0.5f * squashY, s * squashX};
        Model& model = (state == CellState::PlayerX) ? pieceModelLight : pieceModelDark;
        // Swap to per-position unique texture
        Texture2D& tex = (state == CellState::PlayerX)
                             ? pieceTexLight[row][col]
                             : pieceTexDark[row][col];
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
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
