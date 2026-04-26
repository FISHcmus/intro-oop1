// Local simulator for the treasure2 server library.
//
// Implements the public API (move, flip) declared in treasure2lib.h plus
// runner control hooks (initJudge, startBob, ...) declared in judge.h.

#include "treasure2lib.h"
#include "judge.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

namespace {

constexpr int MAX_N = 2048;

int g_N;
bool g_isCycle;
int g_treasure;
int g_adj[MAX_N][2];
bool g_lights[MAX_N];
int g_floor[MAX_N];
int g_cur;
int g_nextFloor;
int g_aliceFlipBudget;
int g_phase;          // 0 = Alice, 1 = Bob
int g_moveCount;
int g_moveLimit;       // 0 = unlimited
std::mt19937 g_rng;
int g_lastDoorOrder[2];
std::vector<int> g_aliceFlipPositions;

void fatal(const char* fmt, int a = 0, int b = 0) {
    std::fprintf(stderr, "JUDGE FATAL: ");
    std::fprintf(stderr, fmt, a, b);
    std::fprintf(stderr, "\n");
    std::abort();
}

std::vector<int> doObserve() {
    int n0 = g_adj[g_cur][0];
    int n1 = g_adj[g_cur][1];
    if (g_rng() & 1u) std::swap(n0, n1);
    g_lastDoorOrder[0] = n0;
    g_lastDoorOrder[1] = n1;
    auto info = [](int n) -> int {
        if (n == -1) return -1;
        return g_floor[n];
    };
    return std::vector<int>{ g_lights[g_cur] ? 1 : 0, info(n0), info(n1) };
}

}  // namespace

// === Public API exposed to solution.cpp via treasure2lib.h ===

std::vector<int> move(int i) {
    if (i != 1 && i != 2) fatal("move(%d) — must be 1 or 2", i);
    int dest = g_lastDoorOrder[i - 1];
    if (dest == -1) fatal("move(%d) crossed boundary at room %d", i, g_cur);
    g_cur = dest;
    if (g_floor[g_cur] == 0) g_floor[g_cur] = g_nextFloor++;
    ++g_moveCount;
    if (g_moveLimit > 0 && g_moveCount > g_moveLimit) {
        fatal("move limit (%d) exceeded — likely infinite loop", g_moveLimit);
    }
    return doObserve();
}

void flip() {
    if (g_phase == 0) {
        if (g_aliceFlipBudget == 0) fatal("Alice exceeded flip budget (>2)");
        --g_aliceFlipBudget;
        g_aliceFlipPositions.push_back(g_cur);
    }
    g_lights[g_cur] = !g_lights[g_cur];
}

// === Runner-facing control API (judge.h) ===

void initJudge(int n, bool isCycle, int treasure,
               const std::vector<bool>& initialLights, std::uint64_t seed) {
    assert(n >= 3 && n <= MAX_N);
    assert(static_cast<int>(initialLights.size()) == n);
    if (isCycle) {
        assert(treasure >= 0 && treasure < n);
    } else {
        assert(treasure >= 1 && treasure <= n - 2);
    }

    g_N = n;
    g_isCycle = isCycle;
    g_treasure = treasure;

    for (int i = 0; i < n; ++i) {
        if (isCycle) {
            g_adj[i][0] = (i - 1 + n) % n;
            g_adj[i][1] = (i + 1) % n;
        } else {
            g_adj[i][0] = (i == 0) ? -1 : i - 1;
            g_adj[i][1] = (i == n - 1) ? -1 : i + 1;
        }
        g_lights[i] = initialLights[i];
        g_floor[i] = 0;
    }

    g_cur = treasure;
    g_floor[g_cur] = 1;
    g_nextFloor = 2;
    g_phase = 0;
    g_aliceFlipBudget = 2;
    g_moveCount = 0;
    g_moveLimit = 1000 * n;  // safety net; runner may override via setMoveLimit
    g_aliceFlipPositions.clear();
    g_rng.seed(seed);
}

void setMoveLimit(int limit) { g_moveLimit = limit; }

void startBob(int bobStart) {
    assert(bobStart >= 0 && bobStart < g_N);
    g_phase = 1;
    for (int i = 0; i < g_N; ++i) g_floor[i] = 0;
    g_cur = bobStart;
    g_floor[g_cur] = 1;
    g_nextFloor = 2;
    g_moveCount = 0;
    // g_moveLimit kept; runner may setMoveLimit() between phases.
}

std::vector<int> getInitialObservation() {
    return doObserve();
}

int getCurRoom() { return g_cur; }
int getAliceFlipsUsed() { return 2 - g_aliceFlipBudget; }
const std::vector<int>& getAliceFlipPositions() { return g_aliceFlipPositions; }
int getMoveCount() { return g_moveCount; }

std::vector<bool> getLightsSnapshot() {
    return std::vector<bool>(g_lights, g_lights + g_N);
}
