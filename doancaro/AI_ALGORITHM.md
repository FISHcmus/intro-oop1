# Caro Game AI Algorithm — Implementation Reference

Last updated: 2026-04-21

## Overview

A single minimax engine with three difficulty tiers. All tiers share the same
pattern-table evaluation and move-ordering heuristic; they differ in search
depth and in whether two optional features (transposition table, opening
book) are enabled.

| Difficulty | `searchDepth` | Transposition table | Opening book |
|---|---|---|---|
| Easy   | 2 | off | off |
| Normal | 3 | off | off |
| Hard   | 4 | on  | on  |

TT and opening book are deliberately gated off for Easy / Normal so the three
tiers are distinguishable — not just three depth knobs on the same pipeline.
Hard is the only tier that plays canonical opening moves and benefits from
move-to-front reordering across the search tree.

## Architecture

```
AIPlayer::getMove(board)
  │
  ├── clear TT, reset debug
  ├── if Hard && moveCount ≤ 6: probe OpeningBook
  │       hit → return book move   (reason = "opening_book")
  │
  ├── candidates = board.getCandidateMoves(radius=2)
  ├── score + sort candidates (scoreMove heuristic)
  ├── if top score ≥ 200 000: return it   (reason = "immediate_win")
  │
  └── minimax α-β at effectiveDepth (opening taper applies)
         │
         ├── (Hard only) TT probe → early return on cutoff
         ├── generate + sort candidates
         ├── (Hard only) hoist TT best-move to front of sorted list
         ├── recurse on each candidate
         └── (Hard only) store {depth, score, flag, bestMove} in TT
```

---

## 1. Difficulty Levels

Player picks Easy / Normal / Hard on the Settings screen.
`aiDepth ∈ {2, 3, 4}` is persisted to `settings.cfg` and passed to
`AIPlayer(searchDepth = aiDepth)` at construction (`Game.cpp`). A single
gate in `AIPlayer.cpp` decides whether TT and opening book are in play:

```cpp
const bool useTT   = (searchDepth >= 4);   // inside minimax()
const bool useBook = (searchDepth >= 4);   // inside getMove()
```

### 1.0 Shared scaffolding (all tiers run this after the book probe)

```
candidates = board.getCandidateMoves(radius = 2)
if candidates.empty()      → return center (SIZE/2, SIZE/2)
if candidates.size() == 1  → return the only option

for each candidate m:
    score = scoreMove(m, aiMark, opponentMark)   ← see §2, §3
sort candidates desc by score

if top score ≥ 200 000:
    reason = "immediate_win"
    return scored[0]
```

### 1.0a Opening taper (all tiers)

To avoid wasting search budget on positions with no tactics yet:

```
if moveCount ≤ 3: effectiveDepth = min(searchDepth, 2)
else if moveCount ≤ 9: effectiveDepth = min(searchDepth, 4)
else: effectiveDepth = searchDepth
```

- **Easy** (d=2): unaffected.
- **Normal** (d=3): drops to d=2 for the first ~2 moves, then its own d=3.
- **Hard** (d=4): drops to d=2 for the first ~2 moves, d=4 thereafter.

When `effectiveDepth < searchDepth`, the debug reason is `"opening_shallow"`
instead of `"minimax"`.

### 1.1 Easy — `searchDepth = 2`

Straight 2-ply minimax α-β. The AI plays its move and evaluates every
single opponent reply, picking the move whose worst-case reply is best.
No TT, no move-to-front hoisting, no opening book.

| Knob | Value | Source |
|---|---|---|
| `searchDepth` | 2 | `SettingsScreen.cpp` (label `"Easy"`) |
| Time budget | none | — |
| Transposition table | disabled | gate `searchDepth >= 4` |
| Opening book | disabled | gate `searchDepth >= 4` |
| Candidate radius | 2 | `Board.cpp` |

**Feel:** sees exactly one counter-move. Blocks direct fours, misses any
fork where the opponent needs two setup moves to win. Beatable with a
standard double-three trap.

### 1.2 Normal — `searchDepth = 3`

3-ply minimax α-β, still without TT or opening book. The extra ply vs. Easy
means the AI sees the opponent's follow-up *after the AI's response*, not
just the immediate reply — it can reason one move beyond its own.

| Knob | Value | Source |
|---|---|---|
| `searchDepth` | 3 | `SettingsScreen.cpp` (label `"Normal"`) |
| Transposition table | disabled | gate `searchDepth >= 4` |
| Opening book | disabled | gate `searchDepth >= 4` |

**Feel:** catches most open-three / closed-four motifs that Easy misses,
but is still vulnerable to multi-move tactical setups and sees no opening
theory. Search cost is roughly `N × (Easy cost)` where N is the average
branching factor (~30-40 candidates), so moves take noticeably longer
than Easy but nowhere near Hard.

### 1.3 Hard — `searchDepth = 4` + TT + opening book

4-ply minimax α-β with two performance features enabled:

1. **Transposition table** (`std::unordered_map<uint64_t, TTEntry>` in
   `AIPlayer`): each visited position is stored keyed by Zobrist hash with
   `{depth, score, flag ∈ {Exact, LowerBound, UpperBound}, bestMove}`. A
   probed entry can either:
   - return immediately (exact score, or bound-tightening that makes
     α ≥ β → cutoff), or
   - hoist its stored `bestMove` to the front of the sorted candidate list
     so α-β prunes harder on the re-search.

   Cleared at the start of every `getMove()` call (TT is per-turn).

2. **Opening book** (`OpeningBook.cpp`): hand-curated table keyed by
   Zobrist hash. 6 base entries × D4 symmetry expansion = 22 stored
   hashes covering all single-stone near-center positions plus the empty
   board. Probed *before* candidate generation — a hit short-circuits
   the entire search.

   Current Tier 1 base entries:

   | Stones | Book move | Meaning |
   |---|---|---|
   | `[]` | (7,7) | Empty board → center |
   | `[(7,7, X)]` | (6,6) | Opponent took center → one-step diagonal |
   | `[(7,6, X)]` | (7,7) | Ortho-adjacent → take center |
   | `[(6,6, X)]` | (7,7) | Diag-adjacent → take center |
   | `[(7,5, X)]` | (7,7) | 2-step ortho → take center |
   | `[(6,5, X)]` | (7,7) | Knight move → take center |

   **Tier 1 covers only the AI's first move** (whether AI opens, or AI
   replies to the human's opening). Once two stones are on the board, book
   lookups always miss and minimax takes over.

| Knob | Value | Source |
|---|---|---|
| `searchDepth` | 4 | `SettingsScreen.cpp` (label `"Hard"`) |
| TT | enabled | gate `searchDepth >= 4` |
| Opening book | enabled, probed when `moveCount ≤ 6` | `AIPlayer.cpp` getMove |
| Book base entries | 6 | `OpeningBook.cpp` |
| Book hashes after D4 expansion | 22 | verified at construction |
| Book symmetry group | D4 (4 rotations × 2 reflections) | `transformRC` |

**Feel:** solid midgame tactics once past the opening; plays canonical
opening moves for the AI's first move. Still bounded by our coarse
pattern-table evaluation — deeper search than d=4 amplifies eval noise
faster than it resolves tactics, which is why we stop here (see §10).

---

## 2. Pattern Table (`initPatternTable`)

A static lookup table with **243 entries** (3^5), pre-computed once.
Each entry scores a 5-cell window from one player's perspective.

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

**Key design choice:** If ANY opponent piece exists in the window, the
score is 0. This means a window can only score for one side — you score
it twice (once as AI, once as opponent) and subtract.

---

## 3. Evaluation Functions

### `evaluate()` — Full Board Evaluation

Scans **every** 5-cell window on the board across 4 directions
(horizontal, vertical, diagonal, anti-diagonal). Avoids duplicate
scanning by only starting lines from perpendicular edges.

```
score = Σ (patternScore[aiWindow] - patternScore[opWindow] × 3/2)
```

**Defense multiplier: 1.5x** — Opponent threats are weighted 50% heavier
than AI opportunities. This makes the AI prioritize blocking over
attacking when scores are otherwise equal.

Example: An opponent's open three (`_OOO_`) scores 1500 × 1.5 = **2250**
on the defense side, clearly outweighing the AI's own open three at
**1500**.

### `computeLocalScore()` — Local Evaluation Around a Cell

Same formula as `evaluate()`, but only scans the **20 windows that
include a specific cell** (5 positions × 4 directions). Used for
incremental score updates in minimax.

Same 1.5x defense multiplier applied.

### `scoreMove()` — Move Ordering Heuristic

Quick evaluation for sorting candidates (searched in order of this
score):

1. Place the move temporarily
2. Check for instant win → return 200,000 (AI win) or 150,000 (opponent
   win via this square)
3. Otherwise, return `abs(computeLocalScore())` — the absolute value
   ensures both offensive AND defensive moves sort high

**Note:** The `abs()` is intentional for move ordering only. It ensures
that a move blocking a big opponent threat (negative local score) is
searched before a neutral move. The actual minimax evaluation preserves
the sign.

---

## 4. Minimax with Alpha-Beta

### Incremental Scoring

Instead of calling `evaluate()` at every leaf node (O(board_size²) per
call), minimax maintains a running `boardScore`:

```
newScore = boardScore - localBefore + localAfter
```

Where `localBefore` is the local score at the move's cell before placing,
and `localAfter` is after placing. This makes leaf evaluation O(1).

### Transposition Table (Hard only)

Enabled only when `searchDepth >= 4`. Uses **Zobrist hashing** — each
(row, col, mark) triple has a random 64-bit key computed at
`Board::initZobrist()`. Board hash = XOR of all placed pieces' keys,
updated incrementally in `placeMove` / `undoMove`.

Stored entries:

```cpp
struct TTEntry {
    int depth;        // search depth when stored
    int score;        // evaluation result
    TTFlag flag;      // Exact | LowerBound | UpperBound
    Move bestMove;    // {-1,-1} sentinel = not stored; used for move ordering
};
```

Probe logic inside `minimax()`:

```
if useTT:
    it = transTable.find(hash)
    if it != end:
        ttBestMove = it->bestMove    ← used later for hoist
        if it->depth ≥ current depth:
            if flag == Exact: return it->score              (cutoff)
            if flag == LowerBound: α = max(α, it->score)
            if flag == UpperBound: β = min(β, it->score)
            if α ≥ β: return it->score                      (cutoff)
```

After the candidate list is sorted by `scoreMove`, if the TT gave us a
`bestMove`, we linear-scan the sorted list and swap it to index 0. This
is robust against hash collisions and changed candidate sets — if the
move isn't in the current candidate list, the swap is a no-op.

Cleared at the start of every `getMove()` call (TT is per-turn, not
persistent across moves — the opponent's move usually invalidates most
prior entries anyway).

Easy and Normal bypass every TT operation — no probe, no store, no
counter increment. The `ttProbes / ttHits / ttStores / ttHoists /
ttCutoffs / ttFinalSize` fields in the debug panel stay at 0 for those
tiers, which is correct.

### Terminal Conditions

```
if winner == aiMark:       return +100000 + depth   (prefer faster wins)
if winner == opponentMark: return -100000 - depth   (prefer slower losses)
if board full:             return 0
if depth == 0:             return boardScore        (leaf: incremental score)
```

---

## 5. Candidate Generation (`Board::getCandidateMoves`)

Returns all empty cells within **radius 2** (Chebyshev distance) of any
existing piece. On an empty board, returns the center cell. This limits
branching factor to ~30-50 moves in typical positions.

Radius 2 is a deliberate choice: radius 1 would miss "gap-of-1" shape
plays (e.g., `X _ X _ X`) that are legitimate tactical moves; radius 3
inflates the branching factor without producing meaningfully better moves
at our evaluation strength.

---

## 6. Opening Book (`OpeningBook.cpp`)

Hard-mode only. A hand-curated table of Zobrist-hash → Move entries
representing canonical opening theory. See §1.3 for the Tier 1 base
entries and the D4 symmetry expansion.

### Symmetry expansion

Each base entry (a list of stones + a book move) is replicated under all
8 D4 transforms of the 15×15 grid:

```
0: identity                     (r, c) → (r, c)
1: rotate 90°                   (r, c) → (c, 14-r)
2: rotate 180°                  (r, c) → (14-r, 14-c)
3: rotate 270°                  (r, c) → (14-c, r)
4: flip horizontal              (r, c) → (r, 14-c)
5: flip vertical                (r, c) → (14-r, c)
6: flip main diagonal           (r, c) → (c, r)
7: flip anti-diagonal           (r, c) → (14-c, 14-r)
```

For each transform we build a fresh `Board`, play the transformed stones
on it, and map `board.getHash() → transformedBookMove`. Positions with
intrinsic symmetry (e.g., a single stone at the exact center) collapse
multiple transforms onto the same hash; the final map value is whichever
transform we wrote last, but since all transformed moves are equivalent
under the board's own symmetry, any of them is a valid response.

### Query

`query(uint64_t hash) → Lookup{bool found; Move move}` — simple
`unordered_map::find`. No std::optional (C++14 constraint), just a POD
struct with a `found` flag.

### What Tier 1 does not cover

- Any position with 2+ stones on the board. The book misses once the AI
  has made its first response and the opponent plays again.
- Any position outside the near-center cluster at move 1 (e.g., opponent
  plays 5 cells out from center). Rare in practice; minimax handles it.

Extending to **Tier 2** would add ~10 three-stone entries transcribed
from Wikipedia's Renju opening page (Kouyoku, Keigetsu, Suigetsu, etc.)
with canonical move-4 replies. Not implemented.

---

## 7. Threat Classification System

Not used in the main search, but available for future use and move
ordering.

### `classifyThreat(board, row, col, mark)`

After placing `mark` at (row,col), checks all 4 directions:
- **Level 4**: 5+ in a row → win
- **Level 3**: 4 in a row, both ends open → open four (unstoppable)
- **Level 2**: 4 in a row, one end open → closed four (one forced block)
- **Level 1**: 3 in a row, both ends open → open three
- **Level 0**: nothing notable

### `findThreats(board, mark)`

Tries every candidate move, classifies each, returns moves with level ≥ 2
sorted by level descending.

### `findDefenses(board, row, col, attackMark)`

After attacker places at (row,col), finds all cells that block the
four-in-a-row (the open endpoints). Deduplicated.

### `findThreatWin(board, attackMark, defendMark, depth)`

Recursive threat search — finds forced wins through continuous fours.
Searches up to `MAX_THREAT_DEPTH = 20`. Currently not called from
`getMove()` — exists as infrastructure for a future VCF/VCT pass.

---

## 8. Debug System

Press **F3** during gameplay to toggle the debug panel. Shows after each
AI move.

| Field | Values |
|---|---|
| Mode | `opening_book`, `immediate_win`, `opening_shallow`, `minimax` |
| Depth | effective search depth that was actually used (0 on book / win short-circuit) |
| Candidates | total candidate moves considered at the root |
| Time | wall-clock search time in ms |
| Nodes | nodes visited inside `minimax()` |
| TT | `hits/probes (%)`, colored **orange** when `ttHits == 0` (indicates Easy/Normal or a TT miss on Hard) |
| Cutoffs / Hoists / Stored | TT instrumentation (Hard only) |
| Chosen | AI's move, highlighted green |
| Top 5 | Best 5 moves ranked by minimax score, with pre-score (scoreMove) and search score |

### Reading the panel

- `Mode: opening_book` + everything else 0 is normal and correct — the AI
  short-circuited the whole search from a known position.
- `Mode: minimax` with `TT: 0/N (0.0%)` in orange means you're probably on
  Normal or Easy (TT gated off). That is *not* a warning on those tiers.
- `Mode: minimax` on Hard with `TT: 0/N (0.0%)` **is** a regression —
  the gate code got bypassed or the map is being cleared mid-search.

### How to diagnose a bad move

1. Play against AI, notice a bad move.
2. Press **Undo** to take back the move.
3. Press **F3** to enable the debug panel.
4. Let the AI replay — check the top 5 list.
5. If the "obvious block" is there with a lower search score → evaluation
   problem (pattern table or defense multiplier).
6. If it's not in the top 5 at all → candidate generation or move
   ordering problem.
7. If depth is lower than expected → opening taper is capping it (check
   `moveCount`).

---

## 9. Known Weaknesses

1. **Horizon at depth 4.** On Hard the AI still can't see forced-win
   sequences longer than ~4 plies. A proper VCF/VCT pass using the
   threat-classification infrastructure in §7 would fix this. Not
   implemented because the previous attempt (reverted 2026-03-21)
   returned wrong moves.

2. **Defense multiplier is a heuristic, not a solution.** The 1.5x
   defense weight helps Hard prioritize blocks, but it doesn't remove
   horizon errors — the AI can still miss threats beyond its search
   depth.

3. **No candidate pruning.** All ~30-50 candidates are searched at every
   node on Hard. Pruning to the top 12-15 per node would allow 2-3 plies
   deeper search in the same time budget. A previous attempt was
   reverted alongside VCF/VCT.

4. **TT is per-turn.** Clearing at the start of each `getMove()` avoids
   stale entries but discards work the opponent's move may not have
   invalidated. Keeping the TT across turns with proper depth/staleness
   checks would help.

5. **Move ordering at internal nodes is expensive.** `scoreMove()` calls
   `computeLocalScore()` which does 20 window lookups. Killer-move and
   history-heuristic ordering would be much cheaper.

6. **Opening book is tiny (Tier 1).** 6 base entries / 22 hashes only
   covers the AI's first move. The AI falls out of book as soon as two
   stones are on the board. Tier 2 (Renju openings) not implemented.

---

## 10. Change History

| Date | Change | Status |
|---|---|---|
| Initial | Basic minimax + alpha-beta | Kept |
| Later | Pattern table evaluation (243 entries) | Kept |
| Later | Iterative deepening + time limits | Reverted (02d1ed1) |
| Later | Transposition table (Zobrist) | Kept, Hard-only |
| Later | Incremental scoring (`computeLocalScore`) | Kept |
| 2026-03-21 | VCF/VCT threat search | Reverted — returned wrong moves |
| 2026-03-21 | Candidate pruning (max 15) | Reverted alongside VCF/VCT |
| 2026-03-21 | Defense multiplier 1.5x | Kept |
| 2026-03-21 | Debug panel (F3) | Kept |
| 2026-03-22 | Board 19x19 → 15x15 | Kept |
| 2026-03-22 | Rapfi engine for Hard mode | Removed (02d1ed1) |
| 2026-03-22 | Settings persistence (`settings.cfg`) | Kept |
| 2026-04-21 | Drop Rapfi, cap depth 4, 2-tier (Easy/Hard) | Superseded (below) |
| 2026-04-21 | TT `bestMove` field + move-to-front hoist | Kept |
| 2026-04-21 | TTFlag enum (Exact/LowerBound/UpperBound) | Kept |
| 2026-04-21 | Debug panel TT instrumentation | Kept |
| 2026-04-21 | Opening book Tier 1 (6 entries × D4 = 22 hashes) | Kept |
| 2026-04-21 | 3-tier difficulty (Easy d=2 / Normal d=3 / Hard d=4) | Kept — current |
| 2026-04-21 | TT + opening book gated Hard-only (`searchDepth >= 4`) | Kept — current |
