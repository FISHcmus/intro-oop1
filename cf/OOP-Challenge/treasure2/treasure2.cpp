// treasure2.cpp — submission file for problem F.
//
// v2 — path: weighted-sum mod N decoder + lex-min canonicalization.
//      cycle: v1 fallback (flip treasure, hunt for unique-ON degree-2 room).
//
// Path strategy (encoder):
//   Alice walks the full path, records all lights, identifies endpoints.
//   She enumerates flip pairs to find one (a, b) with at most 2 elements
//   such that L' = L XOR e_a XOR e_b satisfies:
//     (i)  L' is non-palindrome (so canonical orientation is unambiguous);
//     (ii) the canonical-orientation weighted sum mod N equals the
//          treasure room's position in that same canonical orientation.
//
// Path strategy (decoder, Bob):
//   Walk full path. Build pattern in chosen frame. Compute lex-min vs
//   reverse to pick canonical. Compute t* = sum(i*L'[i]) mod N under
//   canonical. Walk to room at distance t* from the canonical endpoint.

#include "treasure2lib.h"

#include <climits>
#include <cstdint>
#include <vector>

using std::vector;

namespace {

constexpr int RANGE = 1100;            // |abs_pos| ≤ RANGE
constexpr int VEC_SIZE = 2 * RANGE;

struct Walker {
    int lights[VEC_SIZE];
    int floorAt[VEC_SIZE];
    int nextFloor;
    int curPos;
    int prevPos;
    int minPos, maxPos;
    bool isCycle;
    int n;

    void reset() {
        for (int i = 0; i < VEC_SIZE; ++i) {
            lights[i] = -1;
            floorAt[i] = 0;
        }
        nextFloor = 2;
        curPos = 0;
        prevPos = INT_MIN;
        minPos = 0;
        maxPos = 0;
        isCycle = false;
        n = 0;
    }

    int idx(int p) const { return p + RANGE; }
    int floorOf(int p) const { return floorAt[idx(p)]; }
    int lightOf(int p) const { return lights[idx(p)]; }

    void recordStart(int light) {
        floorAt[idx(0)] = 1;
        lights[idx(0)] = light;
    }

    int prevFloor() const {
        if (prevPos == INT_MIN) return 0;
        return floorAt[idx(prevPos)];
    }

    void doMove(int door, int direction, vector<int>& s) {
        int newPos = curPos + direction;
        prevPos = curPos;
        curPos = newPos;
        s = ::move(door);
        if (floorAt[idx(curPos)] == 0) {
            floorAt[idx(curPos)] = nextFloor++;
            lights[idx(curPos)] = s[0];
            if (curPos < minPos) minPos = curPos;
            if (curPos > maxPos) maxPos = curPos;
        }
    }
};

// Pick a forward door (away from prev). 0 = none (boundary in this dir).
int forwardDoor(const vector<int>& s, int prevF) {
    if (s[1] != -1 && s[1] != prevF) return 1;
    if (s[2] != -1 && s[2] != prevF) return 2;
    return 0;
}

// Pick the back door (matching prev). 0 = none.
int backDoor(const vector<int>& s, int prevF) {
    if (s[1] == prevF) return 1;
    if (s[2] == prevF) return 2;
    return 0;
}

// Walk from current state until forward is blocked (boundary) or would
// re-enter start (cycle close). Returns true on boundary, false on cycle.
// Cycle detection should only run in the first phase of walking — in path
// phase 2 the walker legitimately crosses the start room and would
// otherwise trip a false positive.
bool walkUntilStop(Walker& w, vector<int>& s, int direction, bool detectCycle) {
    while (true) {
        int pf = w.prevFloor();
        int fwd = forwardDoor(s, pf);
        if (fwd == 0) return true;            // boundary reached
        if (detectCycle && s[fwd] == 1 && w.curPos != 0) {
            w.isCycle = true;
            return false;
        }
        w.doMove(fwd, direction, s);
    }
}

// Navigate from current pos to targetPos along a known path/cycle segment.
// Walker must already have floor info for every room between cur and target.
void walkTo(Walker& w, vector<int>& s, int targetPos) {
    while (w.curPos != targetPos) {
        int dir = (targetPos > w.curPos) ? +1 : -1;
        int wantFloor = w.floorOf(w.curPos + dir);
        int door = 0;
        if (s[1] == wantFloor) door = 1;
        else if (s[2] == wantFloor) door = 2;
        else {
            int pf = w.prevFloor();
            int fwd = forwardDoor(s, pf);
            if (fwd != 0) door = fwd;
            else door = backDoor(s, pf);
        }
        if (door == 0) return;  // shouldn't happen
        w.doMove(door, dir, s);
    }
}

// === Alice ===

void exploreAlice(Walker& w, vector<int>& s) {
    w.reset();
    w.recordStart(s[0]);

    // Treasure has degree 2 — both doors are valid. Pick door 1.
    int firstDoor = 1;
    if (s[1] == -1) firstDoor = 2;
    int dir = +1;
    w.doMove(firstDoor, dir, s);

    walkUntilStop(w, s, dir, /*detectCycle=*/true);

    if (w.isCycle) {
        w.n = w.maxPos + 1;
        return;
    }

    // Path: turn around at boundary 1, walk to boundary 2.
    int back = backDoor(s, w.prevFloor());
    dir = -1;
    w.doMove(back, dir, s);
    walkUntilStop(w, s, dir, /*detectCycle=*/false);

    w.n = w.maxPos - w.minPos + 1;
}

void aliceSolvePath(Walker& w, vector<int>& s) {
    int N = w.n;
    int leftAbs = w.minPos;          // abs_pos of one endpoint
    int t_A = 0 - leftAbs;           // treasure's distance from left endpoint

    vector<int> L(N);
    for (int i = 0; i < N; ++i) L[i] = w.lightOf(leftAbs + i);

    // Sum mod N (under "left as A" orientation) and popcount.
    int64_t S_A = 0;
    int popcnt = 0;
    for (int i = 0; i < N; ++i) {
        S_A += (int64_t)i * L[i];
        popcnt += L[i];
    }
    S_A %= N;

    // Try a candidate flip pair {a, b} (use -1 for "no flip in this slot").
    auto tryPair = [&](int a, int b) -> bool {
        int64_t newS = S_A;
        int newPop = popcnt;
        if (a >= 0) {
            int64_t da = (L[a] == 0) ? a : (N - a) % N;
            newS = (newS + da) % N;
            newPop += (L[a] == 0) ? 1 : -1;
        }
        if (b >= 0 && b != a) {
            int64_t db = (L[b] == 0) ? b : (N - b) % N;
            newS = (newS + db) % N;
            newPop += (L[b] == 0) ? 1 : -1;
        }

        auto Lp = [&](int i) -> int {
            int v = L[i];
            if (i == a) v ^= 1;
            if (i == b && b != a) v ^= 1;
            return v;
        };

        // Find first asymmetric position from i=0.
        int firstDiff = -1;
        for (int i = 0; i < N / 2; ++i) {
            if (Lp(i) != Lp(N - 1 - i)) { firstDiff = i; break; }
        }
        if (firstDiff == -1) return false;          // palindrome → ambiguous

        bool canonicalIsA = (Lp(firstDiff) == 0);

        if (canonicalIsA) {
            return (int)newS == t_A;
        } else {
            int64_t S_B = (((int64_t)(N - 1) * newPop) % N - newS + (int64_t)N * 2) % N;
            return (int)S_B == (N - 1 - t_A);
        }
    };

    int chosenA = -1, chosenB = -1;
    bool found = false;

    if (tryPair(-1, -1)) found = true;

    if (!found) {
        for (int i = 0; i < N && !found; ++i) {
            if (tryPair(i, -1)) { chosenA = i; chosenB = -1; found = true; }
        }
    }

    if (!found) {
        for (int i = 0; i < N && !found; ++i) {
            for (int j = i + 1; j < N && !found; ++j) {
                if (tryPair(i, j)) { chosenA = i; chosenB = j; found = true; }
            }
        }
    }

    if (!found) {
        // Shouldn't happen for typical L. Last-resort: leave as-is.
        return;
    }

    if (chosenA >= 0) {
        walkTo(w, s, leftAbs + chosenA);
        flip();
    }
    if (chosenB >= 0) {
        walkTo(w, s, leftAbs + chosenB);
        flip();
    }
}

void aliceSolveCycle(Walker& w, vector<int>& s) {
    // v1 fallback: walk back to treasure (curPos = 0) and flip.
    walkTo(w, s, 0);
    flip();
}

// === Bob ===

void exploreBob(Walker& w, vector<int>& s) {
    w.reset();
    w.recordStart(s[0]);

    int firstDoor;
    if (s[1] == -1 && s[2] == -1) firstDoor = 1;     // shouldn't happen
    else if (s[1] == -1)          firstDoor = 2;
    else if (s[2] == -1)          firstDoor = 1;
    else                          firstDoor = 1;

    int dir = +1;
    w.doMove(firstDoor, dir, s);

    walkUntilStop(w, s, dir, /*detectCycle=*/true);

    if (w.isCycle) {
        w.n = w.maxPos + 1;
        return;
    }

    // Path: turn around and walk to other boundary.
    int back = backDoor(s, w.prevFloor());
    dir = -1;
    w.doMove(back, dir, s);
    walkUntilStop(w, s, dir, /*detectCycle=*/false);

    w.n = w.maxPos - w.minPos + 1;
}

void bobSolvePath(Walker& w, vector<int>& s) {
    int N = w.n;
    int leftAbs = w.minPos;
    int rightAbs = w.maxPos;

    vector<int> L(N);
    for (int i = 0; i < N; ++i) L[i] = w.lightOf(leftAbs + i);

    int firstDiff = -1;
    for (int i = 0; i < N / 2; ++i) {
        if (L[i] != L[N - 1 - i]) { firstDiff = i; break; }
    }

    int targetAbs;
    if (firstDiff == -1) {
        // Palindrome → fallback: nearest interior ON room.
        targetAbs = leftAbs + 1;
        for (int p = leftAbs + 1; p <= rightAbs - 1; ++p) {
            if (w.lightOf(p) == 1) { targetAbs = p; break; }
        }
    } else {
        bool canonicalIsA = (L[firstDiff] == 0);
        int64_t S_A = 0;
        int popcnt = 0;
        for (int i = 0; i < N; ++i) {
            S_A += (int64_t)i * L[i];
            popcnt += L[i];
        }
        S_A %= N;

        int decodedDist;
        int decodeFrom;
        if (canonicalIsA) {
            decodedDist = (int)S_A;
            decodeFrom = leftAbs;
            targetAbs = decodeFrom + decodedDist;
        } else {
            int64_t S_B = (((int64_t)(N - 1) * popcnt) % N - S_A + (int64_t)N * 2) % N;
            decodedDist = (int)S_B;
            decodeFrom = rightAbs;
            targetAbs = decodeFrom - decodedDist;
        }
    }

    walkTo(w, s, targetAbs);
}

void bobSolveCycle(Walker& w, vector<int>& s) {
    int N = w.n;
    // v1: walk to first ON room (cycle: every room is degree-2).
    int targetAbs = w.curPos;
    bool found = false;
    for (int p = 0; p < N; ++p) {
        if (w.lightOf(p) == 1) { targetAbs = p; found = true; break; }
    }
    if (!found) return;
    // For cycle, any direction works; walkTo will step in the linear-frame
    // sense which may take the long way. Acceptable for v1 fallback.
    walkTo(w, s, targetAbs);
}

}  // namespace

extern "C" void resetSolution() {
    // Walker is stack-local in each solver, no globals to reset.
}

void solveAlice(vector<int> s) {
    Walker w;
    exploreAlice(w, s);
    if (w.isCycle) aliceSolveCycle(w, s);
    else           aliceSolvePath(w, s);
}

void solveBob(vector<int> s) {
    Walker w;
    exploreBob(w, s);
    if (w.isCycle) bobSolveCycle(w, s);
    else           bobSolvePath(w, s);
}
