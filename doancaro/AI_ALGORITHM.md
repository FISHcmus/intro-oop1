# Caro Game AI Algorithm — Implementation Reference

Last updated: 2026-03-21

## Overview

The AI uses **minimax with alpha-beta pruning**, enhanced by:
- Pattern table evaluation (5-cell sliding window)
- Incremental board scoring (avoid full re-evaluation)
- Transposition table (Zobrist hashing)
- Iterative deepening with time limits
- Move ordering heuristic
- Defense-weighted evaluation (1.5x multiplier)
- Threat classification system (used for move ordering, not search)

**What we do NOT use** (tried and reverted):
- VCF/VCT threat-space search — caused the AI to return wrong moves (opponent's attack square instead of blocking square). Reverted.
- Candidate pruning — removed alongside VCF/VCT revert.

---

## Architecture

```
getMove()
  ├── scoreMove() × N candidates  →  move ordering
  ├── Immediate win check (score ≥ 200000)
  ├── Easy mode (depth ≤ 2): direct minimax
  └── Medium/Hard: iterativeDeepening()
        └── minimax() with alpha-beta
              ├── Terminal check (winner/full/depth=0)
              ├── Transposition table probe
              ├── scoreMove() × candidates  →  ordering at each node
              └── Recursive search with incremental scoring
```

---

## 1. Difficulty Levels

| Difficulty | searchDepth | Time Limit | Method |
|---|---|---|---|
| Easy | 2 | None | Direct minimax, all candidates |
| Medium | 4 | 1000ms | Iterative deepening (depth 2→4) |
| Hard | 6 | 3000ms | Iterative deepening (depth 2→4→6) |

---

## 2. Pattern Table (`initPatternTable`)

A static lookup table with **243 entries** (3^5), pre-computed once. Each entry scores a 5-cell window from one player's perspective.

### Encoding

Each cell in a 5-cell window is encoded as:
- `0` = empty
- `1` = self (the player being evaluated)
- `2` = opponent

Index = `c0 + c1×3 + c2×9 + c3×27 + c4×81`

### Score Values

| Pattern | Score | Description |
|---|---|---|
| 5 self | 100,000 | Win |
| 4 self, 0 opponent | 15,000 | One move from winning |
| 3 consecutive, both ends empty | 1,500 | Open three (`_SSS_`) |
| 3 consecutive, one end empty | 1,000 | Half-open three (`SSS__`) |
| 3 with one gap | 900 | e.g., `SS_S_`, `S_SS_` |
| 3 spread (`S_S_S`) | 400 | Weak three |
| 2 consecutive | 100 | Pair |
| 2 non-consecutive | 50 | Spread pair |
| 1 self | 10 | Single piece |
| Any opponent in window | 0 | Window blocked, no value |

**Key design choice:** If ANY opponent piece exists in the window, the score is 0. This means a window can only score for one side — you score it twice (once as AI, once as opponent) and subtract.

---

## 3. Evaluation Functions

### `evaluate()` — Full Board Evaluation

Scans **every** 5-cell window on the board across 4 directions (horizontal, vertical, diagonal, anti-diagonal). Avoids duplicate scanning by only starting lines from perpendicular edges.

```
score = Σ (patternScore[aiWindow] - patternScore[opWindow] × 3/2)
```

**Defense multiplier: 1.5x** — Opponent threats are weighted 50% heavier than AI opportunities. This makes the AI prioritize blocking over attacking when scores are otherwise equal.

Example: An opponent's open three (`_OOO_`) scores 1500 × 1.5 = **2250** on the defense side, clearly outweighing the AI's own open three at **1500**.

### `computeLocalScore()` — Local Evaluation Around a Cell

Same formula as `evaluate()`, but only scans the **20 windows that include a specific cell** (5 positions × 4 directions). Used for incremental score updates in minimax.

Same 1.5x defense multiplier applied.

### `scoreMove()` — Move Ordering Heuristic

Quick evaluation for sorting candidates (searched in order of this score):

1. Place the move temporarily
2. Check for instant win → return 200,000 (AI win) or 150,000 (opponent win via this square)
3. Otherwise, return `abs(computeLocalScore())` — the absolute value ensures both offensive AND defensive moves sort high

**Note:** The `abs()` is intentional for move ordering only. It ensures that a move blocking a big opponent threat (negative local score) is searched before a neutral move. The actual minimax evaluation preserves the sign.

---

## 4. Minimax with Alpha-Beta

### Incremental Scoring

Instead of calling `evaluate()` at every leaf node (O(board_size²) per call), minimax maintains a running `boardScore`:

```
newScore = boardScore - localBefore + localAfter
```

Where `localBefore` is the local score at the move's cell before placing, and `localAfter` is after placing. This makes leaf evaluation O(1).

### Transposition Table

Uses **Zobrist hashing** — each (row, col, mark) triple has a random 64-bit key. Board hash = XOR of all placed pieces' keys. Stored entries have:
- `depth` — search depth when stored
- `score` — evaluation result
- `flag` — 0=exact, 1=lower bound (failed high), 2=upper bound (failed low)

Probe logic: only use stored result if `entry.depth >= current_depth`. Handles exact scores and bound tightening.

Table is cleared between iterative deepening depths.

### Terminal Conditions

```
if winner == aiMark:     return +100000 + depth  (prefer faster wins)
if winner == opponentMark: return -100000 - depth  (prefer slower losses)
if board full:           return 0
if depth == 0:           return boardScore
```

---

## 5. Iterative Deepening

Searches depth 2, then 4, then 6 (for hard mode), stopping when time runs out.

- Only accepts results from **fully completed** depth iterations
- If depth 4 completes but depth 6 runs out of time, uses depth 4 result
- Transposition table cleared between depths (could be improved)
- Time checked before each candidate move evaluation

---

## 6. Candidate Generation (`Board::getCandidateMoves`)

Returns all empty cells within **radius 2** of any existing piece. On an empty board, returns the center cell. This limits branching factor to ~30-50 moves in typical positions.

---

## 7. Threat Classification System

Not used in the main search, but available for future use and move ordering.

### `classifyThreat(board, row, col, mark)`

After placing `mark` at (row,col), checks all 4 directions:
- **Level 4**: 5+ in a row → win
- **Level 3**: 4 in a row, both ends open → open four (unstoppable)
- **Level 2**: 4 in a row, one end open → closed four (one forced block)
- **Level 1**: 3 in a row, both ends open → open three
- **Level 0**: nothing notable

### `findThreats(board, mark)`

Tries every candidate move, classifies each, returns moves with level ≥ 2 sorted by level descending.

### `findDefenses(board, row, col, attackMark)`

After attacker places at (row,col), finds all cells that block the four-in-a-row (the open endpoints). Deduplicated.

### `findThreatWin(board, attackMark, defendMark, depth)`

Recursive threat search — finds forced wins through continuous fours. Searches up to `MAX_THREAT_DEPTH = 20`. Currently not called from `getMove()` — exists as infrastructure.

---

## 8. Debug System

Press **F3** during gameplay to toggle the debug panel. Shows after each AI move:

| Field | Description |
|---|---|
| Mode | `iterative_deepening`, `easy_mode`, or `immediate_win` |
| Depth | Deepest fully completed search depth |
| Candidates | Total candidate moves considered |
| Time | Search time in milliseconds |
| Top 5 | Best 5 moves ranked by minimax score, with pre-score (ordering) and search score (minimax) |
| Chosen | Highlighted in green |

### How to Use for Debugging

1. Play against AI, notice a bad move
2. Press **Undo** to take back the move
3. Press **F3** to enable debug panel
4. Let the AI replay — check the scores
5. If the "obvious block" has a lower search score → evaluation problem
6. If it's not in the top 5 → candidate generation or move ordering problem
7. If depth is low → time limit hit too early

---

## 9. Known Weaknesses

1. **No forced-win detection**: Minimax with depth 4-6 can't see wins/threats beyond ~6 plies. A proper VCF/VCT implementation would fix this but previous attempt was buggy.

2. **Defense multiplier is a band-aid**: The 1.5x defense weight helps but doesn't solve fundamental horizon problems. The AI can still miss threats beyond its search depth.

3. **No candidate pruning**: All ~30-50 candidates are searched at every node. Pruning to 12-15 would allow 2-3 plies deeper search in the same time budget.

4. **Transposition table cleared between depths**: Keeping it across depths (with proper depth checks) would speed up iterative deepening.

5. **Move ordering at internal nodes is expensive**: `scoreMove()` calls `computeLocalScore()` which does 20 window lookups. A cheaper ordering (e.g., killer moves, history heuristic) would help.

---

## 10. Change History

| Date | Change | Status |
|---|---|---|
| Initial | Basic minimax + alpha-beta | Kept |
| Later | Pattern table evaluation (replaced evaluateDirection) | Kept |
| Later | Iterative deepening + time limits | Kept |
| Later | Transposition table (Zobrist) | Kept |
| Later | Incremental scoring (computeLocalScore) | Kept |
| 2026-03-21 | VCF/VCT threat search | **Reverted** — returned wrong moves |
| 2026-03-21 | Candidate pruning (max 15) | **Reverted** — alongside VCF/VCT |
| 2026-03-21 | Defense multiplier 1.5x | Kept |
| 2026-03-21 | Debug panel (F3) | Kept |
| 2026-03-21 | Undo button | Kept |
