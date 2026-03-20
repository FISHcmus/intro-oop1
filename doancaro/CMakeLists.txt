cmake_minimum_required(VERSION 3.16)
project(CaroGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Build speed optimizations ---

# 1. ccache: cache compiled objects (5-10x faster rebuilds)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    message(STATUS "ccache found: ${CCACHE_PROGRAM}")
endif()

# 2. mold linker: 3-8x faster linking (native builds only — mold doesn't support cross-compilation)
if(NOT CMAKE_CROSSCOMPILING)
    find_program(MOLD_PROGRAM mold)
    if(MOLD_PROGRAM)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
        message(STATUS "mold linker found: ${MOLD_PROGRAM}")
    endif()
endif()

# 3. Compiler flags for faster builds
add_compile_options(-pipe)                # use pipes instead of temp files
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-gsplit-dwarf)    # split debug info for faster linking
endif()

# --- raylib via FetchContent ---
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
    endif()
endif()

# --- Source files ---
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Player.cpp
    src/AIPlayer.cpp
    src/Renderer.cpp
    src/MenuScreen.cpp
    src/GameScreen.cpp
    src/SettingsScreen.cpp
    src/SaveLoadScreen.cpp
    src/AudioManager.cpp
    src/FileManager.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} raylib)

# 4. Precompiled headers: parse shared headers once
# Note: raylib.h excluded from PCH — it's found via target include dirs
# which PCH doesn't resolve with quoted includes. Use angle-bracket STL only.
target_precompile_headers(${PROJECT_NAME} PRIVATE
    <vector>
    <string>
    <cmath>
    <cstdio>
)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copying assets to build directory"
)

# --- Tests (Catch2 v3) ---
option(BUILD_TESTS "Build unit tests" ON)
if (BUILD_TESTS)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    # Game logic library (no main, no raylib-dependent files)
    set(GAME_LOGIC_SOURCES
        src/Board.cpp
        src/Player.cpp
        src/AIPlayer.cpp
    )
    add_library(GameLogic STATIC ${GAME_LOGIC_SOURCES})
    target_include_directories(GameLogic PUBLIC src)

    add_executable(CaroTests tests/test_board.cpp tests/test_ai.cpp)
    target_link_libraries(CaroTests PRIVATE GameLogic Catch2::Catch2WithMain)

    # PCH for test files
    target_precompile_headers(CaroTests PRIVATE
        <catch2/catch_test_macros.hpp>
        <vector>
        <string>
    )

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(CaroTests)
endif()
