// Local judge runner for problem F (treasure2).
//
// Drives the simulator (treasure2lib_local.cpp) through random or exhaustive
// cases against the contestant's solution.cpp. Reports pass / fail summary
// and dumps full state on each failure.

#include "judge.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <vector>

// --- Solution forward decls (from solution.cpp) ---
extern void solveAlice(std::vector<int> s);
extern void solveBob(std::vector<int> s);

// Optional reset hook — solution.cpp may override the weak no-op here.
extern "C" __attribute__((weak)) void resetSolution();
extern "C" void resetSolution() {}

namespace {

struct Args {
    std::uint64_t seed = 42;
    int n = 1026;
    int cases = 100;
    bool exhaustive = false;
    std::string lightsMode = "random";  // random|zero|one|alt|palin|all
    bool lightsSet = false;             // true if user passed --lights
    int moveLimit = 0;                  // 0 = use judge safety net (1000*n)
    bool verbose = false;
};

void usage(const char* prog) {
    std::fprintf(stderr,
        "Usage: %s [options]\n"
        "  --seed N           RNG seed (default 42)\n"
        "  --n N              Number of rooms (default 1026)\n"
        "  --cases N          Number of random cases (default 100)\n"
        "  --exhaustive       Iterate ALL (struct, treasure, bobStart) for n=10\n"
        "                     Lights chosen per --lights (default 'all' = every 2^n mask)\n"
        "  --lights MODE      random|zero|one|alt|palin|all\n"
        "                       random   bits per case (default for --cases)\n"
        "                       all      iterate every 2^n mask (default for --exhaustive)\n"
        "  --move-limit N     Per-phase move limit (default = judge safety net 1000*n)\n"
        "  --verbose          Print every case (default: only failures + summary)\n"
        "  -h, --help         Show this help\n",
        prog);
}

// Returns 1 = handled, 0 = not a value flag, -1 = error.
int parseValueFlag(int& i, int argc, char** argv, const std::string& s, Args& a) {
    auto need = [&]() -> const char* {
        if (i + 1 >= argc) {
            std::fprintf(stderr, "missing value for %s\n", s.c_str());
            return nullptr;
        }
        return argv[++i];
    };
    if (s == "--seed")        { auto v = need(); if (!v) return -1; a.seed = std::strtoull(v, nullptr, 10); return 1; }
    if (s == "--n")           { auto v = need(); if (!v) return -1; a.n = std::atoi(v); return 1; }
    if (s == "--cases")       { auto v = need(); if (!v) return -1; a.cases = std::atoi(v); return 1; }
    if (s == "--lights")      { auto v = need(); if (!v) return -1; a.lightsMode = v; a.lightsSet = true; return 1; }
    if (s == "--move-limit")  { auto v = need(); if (!v) return -1; a.moveLimit = std::atoi(v); return 1; }
    return 0;
}

bool parseOne(int& i, int argc, char** argv, Args& a) {
    std::string s = argv[i];
    int r = parseValueFlag(i, argc, argv, s, a);
    if (r == 1) return true;
    if (r == -1) return false;
    if (s == "--exhaustive") { a.exhaustive = true; return true; }
    if (s == "--verbose")    { a.verbose = true; return true; }
    if (s == "-h" || s == "--help") { usage(argv[0]); std::exit(0); }
    std::fprintf(stderr, "unknown arg %s\n", s.c_str());
    usage(argv[0]);
    return false;
}

bool parseArgs(int argc, char** argv, Args& a) {
    for (int i = 1; i < argc; ++i) {
        if (!parseOne(i, argc, argv, a)) return false;
    }
    return true;
}

struct Case {
    bool isCycle;
    int treasure;
    int bobStart;
    std::vector<bool> lights;
};

std::vector<bool> buildLights(const std::string& mode, int n, std::mt19937_64& rng) {
    std::vector<bool> L(n, false);
    if (mode == "zero")        { /* all false */ }
    else if (mode == "one")    { for (int i = 0; i < n; ++i) L[i] = true; }
    else if (mode == "alt")    { for (int i = 0; i < n; ++i) L[i] = (i & 1); }
    else if (mode == "palin")  {
        for (int i = 0; i < n / 2; ++i) L[i] = (rng() & 1ULL) != 0;
        for (int i = 0; i < n / 2; ++i) L[n - 1 - i] = L[i];
        if (n & 1) L[n / 2] = (rng() & 1ULL) != 0;
    } else { /* random */
        for (int i = 0; i < n; ++i) L[i] = (rng() & 1ULL) != 0;
    }
    return L;
}

Case genCase(int n, const std::string& mode, std::mt19937_64& rng) {
    Case c;
    c.isCycle = (rng() & 1ULL) != 0;
    if (c.isCycle) {
        c.treasure = static_cast<int>(rng() % static_cast<std::uint64_t>(n));
    } else {
        c.treasure = 1 + static_cast<int>(rng() % static_cast<std::uint64_t>(n - 2));
    }
    c.bobStart = static_cast<int>(rng() % static_cast<std::uint64_t>(n));
    c.lights = buildLights(mode, n, rng);
    return c;
}

std::string lightsToString(const std::vector<bool>& L) {
    std::string s; s.reserve(L.size());
    for (bool b : L) s += (b ? '1' : '0');
    return s;
}

std::string lightsTruncated(const std::vector<bool>& L) {
    if (L.size() <= 64) return lightsToString(L);
    std::string s;
    for (int i = 0; i < 32; ++i) s += (L[i] ? '1' : '0');
    s += "...";
    int n = static_cast<int>(L.size());
    for (int i = n - 32; i < n; ++i) s += (L[i] ? '1' : '0');
    return s;
}

int hammingDist(const std::vector<bool>& a, const std::vector<bool>& b) {
    int c = 0;
    for (std::size_t i = 0; i < a.size(); ++i) if (a[i] != b[i]) ++c;
    return c;
}

void dumpFailure(int caseIdx, const Case& c, const std::vector<bool>& finalLights,
                 const std::vector<int>& flipPos, int actualRoom) {
    std::printf("Case %d  [%s, n=%d, t=%d, bobStart=%d]\n",
        caseIdx, c.isCycle ? "cycle" : "path",
        static_cast<int>(c.lights.size()), c.treasure, c.bobStart);
    std::printf("  Initial: %s\n", lightsTruncated(c.lights).c_str());
    std::printf("  Final:   %s   (Hamming=%d)\n",
        lightsTruncated(finalLights).c_str(), hammingDist(c.lights, finalLights));
    std::printf("  Alice flipped at: [");
    for (std::size_t i = 0; i < flipPos.size(); ++i) {
        std::printf("%s%d", i ? ", " : "", flipPos[i]);
    }
    std::printf("] (%d flip%s)\n", static_cast<int>(flipPos.size()),
        flipPos.size() == 1 ? "" : "s");
    std::printf("  Bob's final room: %d   FAIL (expected %d)\n", actualRoom, c.treasure);
}

void dumpSuccess(int caseIdx, const Case& c, int flips) {
    std::printf("Case %d  [%s, n=%d, t=%d, bs=%d] flips=%d  OK\n",
        caseIdx, c.isCycle ? "cycle" : "path",
        static_cast<int>(c.lights.size()), c.treasure, c.bobStart, flips);
}

bool runCase(const Args& a, int caseIdx, const Case& c) {
    std::uint64_t caseSeed = a.seed * 1000003ULL + static_cast<std::uint64_t>(caseIdx);

    resetSolution();
    initJudge(static_cast<int>(c.lights.size()), c.isCycle, c.treasure, c.lights, caseSeed);
    if (a.moveLimit > 0) setMoveLimit(a.moveLimit);

    auto initObs = getInitialObservation();
    solveAlice(initObs);
    auto flipPos = getAliceFlipPositions();
    auto finalLights = getLightsSnapshot();

    resetSolution();
    startBob(c.bobStart);
    if (a.moveLimit > 0) setMoveLimit(a.moveLimit);
    auto bobObs = getInitialObservation();
    solveBob(bobObs);

    int finalRoom = getCurRoom();
    bool ok = (finalRoom == c.treasure);

    if (!ok) {
        dumpFailure(caseIdx, c, finalLights, flipPos, finalRoom);
    } else if (a.verbose) {
        dumpSuccess(caseIdx, c, static_cast<int>(flipPos.size()));
    }
    return ok;
}

int runRandom(const Args& a) {
    std::mt19937_64 rng(a.seed);
    int passed = 0;
    for (int i = 0; i < a.cases; ++i) {
        Case c = genCase(a.n, a.lightsMode, rng);
        if (runCase(a, i, c)) ++passed;
    }
    std::printf("\n%d / %d passed (%.2f%%)\n",
        passed, a.cases, 100.0 * passed / a.cases);
    return passed == a.cases ? 0 : 1;
}

// Exhaustive over (struct, treasure, bobStart). Lights determined by mode:
//   "all"     → also iterate over all 2^n masks
//   else      → use buildLights(mode, ...) once per (struct, treasure, bobStart)
int runExhaustive(const Args& a) {
    if (a.n != 10) {
        std::fprintf(stderr, "--exhaustive requires --n 10\n");
        return 2;
    }
    int n = 10;
    bool allMasks = (a.lightsMode == "all");
    int total = 0, passed = 0, caseIdx = 0;
    std::mt19937_64 rng(a.seed);

    for (int isCycle = 0; isCycle < 2; ++isCycle) {
        int tLo = isCycle ? 0 : 1;
        int tHi = isCycle ? n : n - 1;
        for (int treasure = tLo; treasure < tHi; ++treasure) {
            int maskLo = 0;
            int maskHi = allMasks ? (1 << n) : 1;
            for (int mask = maskLo; mask < maskHi; ++mask) {
                std::vector<bool> L;
                if (allMasks) {
                    L.assign(n, false);
                    for (int i = 0; i < n; ++i) L[i] = ((mask >> i) & 1) != 0;
                } else {
                    L = buildLights(a.lightsMode, n, rng);
                }
                for (int bobStart = 0; bobStart < n; ++bobStart) {
                    Case c{static_cast<bool>(isCycle), treasure, bobStart, L};
                    if (runCase(a, caseIdx, c)) ++passed;
                    ++total;
                    ++caseIdx;
                }
            }
        }
    }
    std::printf("\n%d / %d passed (%.3f%%)\n",
        passed, total, 100.0 * passed / total);
    return passed == total ? 0 : 1;
}

}  // namespace

int main(int argc, char** argv) {
    Args a;
    if (!parseArgs(argc, argv, a)) return 2;
    // Default lights mode for exhaustive is "all" (iterate every 2^n mask).
    if (a.exhaustive && !a.lightsSet) a.lightsMode = "all";
    if (a.exhaustive) return runExhaustive(a);
    return runRandom(a);
}
