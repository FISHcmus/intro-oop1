# Caro Game AI Algorithm — Implementation Reference

Last updated: 2026-04-21

## Overview

Three difficulty tiers, but **two different algorithms**. Easy is a pure
rule-based greedy one-ply picker (no tree search). Normal and Hard both run
minimax α-β with the transposition table always enabled; only the search
depth and the opening book differ.

| Difficulty | Algorithm | `searchDepth` | TT | Opening book |
|---|---|---|---|---|
| Easy   | Greedy one-ply (Option C) | — (no tree) | n/a | off |
| Normal | Minimax α-β                | 2           | on  | off |
| Hard   | Minimax α-β                | 3           | on  | on  |

**Why this split** — previously all three tiers were the same minimax engine
with different depths (d=2 / d=3 / d=4). Two problems: (1) d=4 took ~120 s
for a single move on mid-game positions, unacceptable. (2) d=2 is still too
strong for a new player — it sees the immediate opponent reply and catches
most one-move threats. We now draw a clean line:

- **Easy** does not search at all — it just scores candidates and picks one
  good-looking move. Blind to opponent replies by construction.
- **Normal and Hard** both search; they differ in how far ahead. TT is on
  for both because the `searchDepth >= 4` gate was arbitrary — TT helps
  wherever minimax runs (see §4).
- Opening book stays Hard-only (hand-curated canonical moves, not a
  depth-feature).

## Architecture

```
AIPlayer::getMove(board)
  │
  ├── clear TT, reset debug
  ├── if Hard && moveCount ≤ 6: probe OpeningBook
  │       hit → return book move   (reason = "opening_book")
  │
  ├── candidates = board.getCandidateMoves(radius=2)
  ├── score candidates (scoreMove heuristic — §3)
  ├── if top score ≥ 200 000: return it   (reason = "immediate_win")
  │
  ├── if Easy tier (no minimax):
  │       return argmax(scoredMoves)      (reason = "greedy_one_ply")
  │
  └── minimax α-β at searchDepth (Normal: 2, Hard: 3)
         │
         ├── TT probe → early return on cutoff
         ├── generate + sort candidates
         ├── hoist TT best-move to front of sorted list
         ├── recurse on each candidate
         └── store {depth, score, flag, bestMove} in TT
```

---

## 1. Difficulty Levels

Player picks Easy / Normal / Hard on the Settings screen.
`aiDepth ∈ {1, 2, 3}` is persisted to `settings.cfg` and passed to
`AIPlayer(searchDepth = aiDepth)` at construction (`Game.cpp`).
A single sentinel separates rule-based Easy from minimax Normal/Hard:

```cpp
// inside getMove()
if (searchDepth < 2) return greedyOnePly(board);   // Easy tier

// inside minimax()
// TT is on whenever minimax runs (no depth gate).
```

Opening book remains Hard-only (hand-curated; not a depth feature).

### 1.0 Shared scaffolding (all tiers run this after the book probe)

```
candidates = board.getCandidateMoves(radius = 2)
if candidates.empty()      → return center (SIZE/2, SIZE/2)
if candidates.size() == 1  → return the only option

for each candidate m:
    score = scoreMove(m, aiMark, opponentMark)   ← see §2, §3
sort candidates desc by score

if top score ≥ 200 000:                          ← opponent-win block
    reason = "immediate_win"                       or AI-win take
    return scored[0]
```

### 1.1 Easy — greedy one-ply (Option C)

**No tree search. No transposition table. No opening book.**

After the shared scaffolding (§1.0), if no immediate win was taken, Easy
evaluates each candidate by its **local-score delta**: measure the local
pattern score at the cell, place the AI stone, measure again, rank by
`after - before`. Argmax wins. One pass, one pick, `O(candidates × windows)`
work.

```
reason = "greedy_one_ply"
delta  = computeLocalScore(after) - computeLocalScore(before)
return argmax(delta, candidates)
```

Why delta and not `scoreMove()`'s absolute score? `scoreMove` returns
`|localAfter|`. At a block cell, `localAfter` is tiny — the AI's lone
stone makes no pattern while the opponent's former 4-in-row window has
collapsed. `scoreMove`-rank would pick a distant pair-builder (score
~140) over a direct block (score ~40). Delta restores sanity:
`localBefore` at a block cell is strongly negative (opponent pattern ×
1.5 defense multiplier), so `after - before` is strongly positive and
wins the argmax.

| Knob | Value | Source |
|---|---|---|
| `searchDepth` | 1 (sentinel, means "no minimax") | `SettingsScreen.cpp` |
| Tree search | none | early return |
| Transposition table | n/a | never reached |
| Opening book | disabled | Hard-only |
| Candidate radius | 2 | `Board.cpp` |

#### Why "greedy one-ply" and not something else — literature review

The design space for a weak Gomoku AI is well-trodden; major prior art:

| Approach | Where seen | Strength |
|---|---|---|
| A. Pure random | tutorials | beats nobody |
| B. Neighborhood-random (random among cells adjacent to any stone) | CarolRameder Level 1 [¹] | beats nobody; *looks* broken |
| **C. Greedy one-ply pattern scoring (this design)** | sen.ltd "Easy" tier [²], Lingz heuristic AI [³] | beats new players; loses to anyone who reads one move ahead |
| D. Option C + top-K random | chess handicap literature [⁴] | adjustable via K |
| E. Rule-based priority list (win / block / block-open-4 / block-open-3 / build) | BGA beginner guide [⁵], Playgama design post [⁶] | stronger than C — every rule added makes it harder |
| Minimax d ≥ 2 (current Normal and Hard) | sen.ltd "Normal" d=2 [²], tournament AIs [⁷] | strong; crushes new players |

Pure random (A) and neighborhood-random (B) are too weak — the AI looks
broken. Option C is the sweet spot because delta captures the three
things the AI must do: **take direct wins** (handled by `scoreMove`'s
200 000 short-circuit in §1.0 before Easy is entered), **block direct
threats** (delta captures the loss of an opponent pattern weighted by
`DEF_MULT = 1.5` in `computeLocalScore`), and **extend own lines**
(delta rises when the AI's own pattern improves). So Option C reliably
takes wins, blocks direct 4-in-a-rows and open-threes, and extends its
own lines — giving it a purposeful feel.

#### What Option C can and cannot see

Because Option C evaluates *only AI's own move*, not the opponent's
reply, it is blind to anything that requires reading one move ahead.
This is the **horizon effect** [⁸] in its extreme form — the search
horizon is zero plies after the move.

```
What Option C handles correctly           What it misses
─────────────────────────────────         ──────────────────────────────
Take a 5-in-a-row (win)                   Forks / double-threat setups
Block opponent's direct 5 or 4            Multi-move tactics (VCF, VCT)
Block opponent's open three               "Opponent extends my threat"
  (DEF_MULT weights the delta)              reasoning
Extend own open-two / open-three          Positional sacrifices
Prefer high-value blocks over own         Anything needing 2+ plies of
  medium-value builds                       lookahead
```

**Concrete consequence** — a human player who sets up a **fork** (two
open threes that intersect at a single cell, or an open four + open
three double-threat) beats Option C every time. Easy evaluates each
block cell independently; each threat looks equally blockable. Whichever
it picks, the other converts to an open four next turn. This is
intentional: Option C can't read two moves ahead, so it can't reason
"if I block here, the other threat wins anyway". A two-threat fork is
the first tactic that beats Easy cleanly.

#### Comparison to the old Easy (minimax d=2)

| Axis | Old Easy (minimax d=2) | New Easy (Option C) |
|---|---|---|
| Tree nodes | ~30 at root × ~30 opponent replies ≈ 900 | ~30 (candidates only) |
| Sees opponent reply | yes (catches one-move traps) | no |
| Catches fork setups | sometimes (d=2 catches shallow ones) | never |
| Blocks open-three proactively | yes (via search, opponent's next extension is evaluated) | only incidentally |
| Blocks direct 5 | yes (via search finding winning opponent move) | yes (via `scoreMove`'s 150 000 hard-coded branch) |
| Think time on 15×15 mid-game | ~10–80 ms | <5 ms |
| Beatable by complete beginner | no (too strong) | yes |

### 1.2 Normal — minimax d=2 + TT

Straight 2-ply minimax α-β. The AI plays its move and evaluates every
opponent reply, picking the move whose worst-case reply is best.
**TT is on** (unlike the old Normal which gated it off); opening book is
still Hard-only.

| Knob | Value | Source |
|---|---|---|
| `searchDepth` | 2 | `SettingsScreen.cpp` (label `"Normal"`) |
| Transposition table | enabled | TT is always on when minimax runs |
| Opening book | disabled | Hard-only |
| Candidate radius | 2 | `Board.cpp` |

**Feel:** sees exactly one counter-move. Catches the "build-open-three →
force-win" tactic that Easy is blind to (d=2 sees the opponent extending
the three on their reply ply and scrambles to block). Still vulnerable to
fork/double-threat setups that need two AI plies to see.

**Why d=2 (not d=3 like old Normal):** d=3 was an arbitrary middle step;
dropping Normal to d=2 and Hard to d=3 (from d=4) gives a cleaner,
faster tier ladder — Normal now thinks in <100 ms per move.

### 1.3 Hard — minimax d=3 + TT + opening book

3-ply minimax α-β with both performance features enabled:

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
| `searchDepth` | 3 | `SettingsScreen.cpp` (label `"Hard"`) |
| TT | enabled | always on when minimax runs |
| Opening book | enabled, probed when `moveCount ≤ 6` | `AIPlayer.cpp` getMove |
| Book base entries | 6 | `OpeningBook.cpp` |
| Book hashes after D4 expansion | 22 | verified at construction |
| Book symmetry group | D4 (4 rotations × 2 reflections) | `transformRC` |

**Feel:** solid tactical play up to 3-move sequences; plays canonical
opening moves for the AI's first move. Catches most two-setup forks
(AI sees its move → opponent's reply → AI's next move).

**Why d=3 (not d=4 like old Hard):** d=4 routinely took ~120 s per move
on mid-game positions (branching factor ~30 → ~30⁴ × leaf cost). With
α-β + TT this gets cut substantially but still hit 60–120 s tails.
That's unshippable for an interactive game. d=3 keeps moves under ~5 s
with TT, and the remaining tactical gap (deep forks) is not worth the
10–20× wait for a casual player.

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

### Transposition Table (always on when minimax runs)

Enabled on **every** minimax call — Normal (d=2) and Hard (d=3) both use
it. Easy never reaches minimax so the TT stays empty on Easy. (The old
`searchDepth >= 4` gate was arbitrary and has been removed — TT helps
wherever α-β runs, including shallow depths where the hoist + cutoff
routinely saves ~20–30% of node expansions.)

Uses **Zobrist hashing** — each (row, col, mark) triple has a random
64-bit key computed at `Board::initZobrist()`. Board hash = XOR of all
placed pieces' keys, updated incrementally in `placeMove` / `undoMove`.

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

Only Easy bypasses every TT operation — it never enters minimax. The
`ttProbes / ttHits / ttStores / ttHoists / ttCutoffs / ttFinalSize`
fields in the debug panel stay at 0 on Easy (correct) and should be
non-zero on Normal and Hard (any zero there is a regression).

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
| Mode | `opening_book`, `immediate_win`, `greedy_one_ply`, `minimax` |
| Depth | search depth actually used (0 on book / win / greedy short-circuits) |
| Candidates | total candidate moves considered at the root |
| Time | wall-clock search time in ms |
| Nodes | nodes visited inside `minimax()` (0 on Easy — no tree) |
| TT | `hits/probes (%)`, colored **orange** when `ttHits == 0` |
| Cutoffs / Hoists / Stored | TT instrumentation (Normal + Hard) |
| Chosen | AI's move, highlighted green |
| Top 5 | Best 5 moves ranked by minimax score, with pre-score (scoreMove) and search score |

### Reading the panel

- `Mode: opening_book` + everything else 0 is normal and correct — the AI
  short-circuited the whole search from a known position (Hard only,
  first move).
- `Mode: greedy_one_ply` + `Nodes: 0` + `TT: 0/0` is normal on Easy — no
  minimax ran; the panel is showing the ranked `scoreMove` output.
- `Mode: immediate_win` is the win/block short-circuit from §1.0 — can
  fire on any tier, skips both minimax and greedy selection.
- `Mode: minimax` with `TT: 0/N (0.0%)` on Normal or Hard **is** a
  regression — TT is always on when minimax runs, so zero probes means
  the map is being cleared mid-search or the probe code is unreachable.

### How to diagnose a bad move

1. Play against AI, notice a bad move.
2. Press **Undo** to take back the move.
3. Press **F3** to enable the debug panel.
4. Let the AI replay — check the top 5 list.
5. If the "obvious block" is there with a lower search score → evaluation
   problem (pattern table or defense multiplier).
6. If it's not in the top 5 at all → candidate generation or move
   ordering problem.
7. If `Mode: greedy_one_ply` and the AI missed an opponent threat that
   required seeing one ply ahead → expected on Easy (see §1.1). Not a
   bug — it's the tier's design.

---

## 9. Known Weaknesses

1. **Easy can't see multi-move tactics.** Option C evaluates only the
   AI's own move (via local-score delta) — the opponent's reply is never
   considered. Direct threats (5-in-a-row, 4-in-a-row, open three) are
   blocked via the defense-weighted delta. But **forks** — two
   simultaneous threats that intersect at a single cell — beat Easy
   every time. Any tactic that needs reading one ply ahead (fork setup,
   "opponent extends my pattern") is missed. This is the horizon
   effect [⁸] at its extreme; it's the design, not a bug.

2. **Horizon at depth 3 on Hard.** The AI still can't see forced-win
   sequences longer than ~3 plies. A proper VCF/VCT pass using the
   threat-classification infrastructure in §7 would fix this. Not
   implemented because the previous attempt (reverted 2026-03-21)
   returned wrong moves. Raising Hard to d=4 was rejected (120 s per
   move — see §1.3).

3. **Defense multiplier is a heuristic, not a solution.** The 1.5x
   defense weight helps Normal and Hard prioritize blocks, but it
   doesn't remove horizon errors — the AI can still miss threats beyond
   its search depth.

4. **No candidate pruning.** All ~30-50 candidates are searched at every
   node on Normal and Hard. Pruning to the top 12-15 per node would
   allow 1-2 plies deeper search in the same time budget. A previous
   attempt was reverted alongside VCF/VCT.

5. **TT is per-turn.** Clearing at the start of each `getMove()` avoids
   stale entries but discards work the opponent's move may not have
   invalidated. Keeping the TT across turns with proper depth/staleness
   checks would help.

6. **Move ordering at internal nodes is expensive.** `scoreMove()` calls
   `computeLocalScore()` which does 20 window lookups. Killer-move and
   history-heuristic ordering would be much cheaper.

7. **Opening book is tiny (Tier 1).** 6 base entries / 22 hashes only
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
| 2026-04-21 | 3-tier difficulty (Easy d=2 / Normal d=3 / Hard d=4) | Superseded (below) |
| 2026-04-21 | TT + opening book gated Hard-only (`searchDepth >= 4`) | Superseded (below) |
| 2026-04-21 | Revise tier split — Easy=greedy one-ply (no minimax), Normal=d2+TT, Hard=d3+TT. Drop `searchDepth >= 4` TT gate (TT always on when minimax runs). Drop d=4 Hard (120 s/move unshippable). Opening book stays Hard-only. | Kept — current |
| 2026-04-21 | Easy defense fix — rank by `computeLocalScore` delta (`after - before`) instead of `scoreMove`'s absolute `\|localAfter\|`. Fixes blindness to opponent open-threes and 4-in-rows; `scoreMove` and minimax untouched. | Kept — current |

---

## 11. References

Algorithm design and weak-AI literature consulted for the 2026-04-21
tier revision:

[¹] Carol Rameder — *Gomoku implementation in Python*,
    github.com/CarolRameder/Gomoku. "Level 1: Random play among cells
    adjacent to any existing stone" — the canonical *neighborhood-random*
    weak AI.

[²] sen.ltd DEV article (2020-era) — *Gomoku AI from Easy to Hard*.
    Describes Easy as **depth-1 minimax** (one-ply evaluation over
    candidates), Normal as d=2, Hard as d=4+ with TT. Depth-1 minimax is
    algorithmically identical to Option C (greedy one-ply pattern
    scoring) — same candidates, same eval, just without the `for each
    candidate: place → recurse(depth=0) → return eval` wrapper.

[³] Lingz — *Heuristic Gomoku AI* write-up describing one-move-ahead
    evaluation with a static pattern table — another concrete deployment
    of Option C.

[⁴] Chess handicap / weak-engine literature — pattern of taking the top
    N scored moves and picking uniformly at random among them to
    deliberately play below full strength (Option D). Shallow survey in
    the Chessprogramming wiki entries on *Skill* and *Strength
    Adjustment*.

[⁵] Board Game Arena — *Gomoku beginner guide*. Teaches the rule-based
    priority ladder (win → block five → block open four → block open
    three → build) that Option E encodes. Good reference for what a
    human beginner considers "obvious" play.

[⁶] Playgama design post — *How to implement a weak but not stupid
    Gomoku AI*. Advocates a rule-priority design (Option E) over
    minimax for casual/mobile deployments; cites responsiveness and
    "AI feels intentional, not random" as the main gains.

[⁷] Gomoku tournament AI survey (general literature, incl. Czajka 2020
    *Gomoku threat taxonomy*, Allis 1993 *Searching for Solutions in
    Games and Artificial Intelligence* §4 on Gomoku/Renju, Junru Wang's
    Yixin/Rapfi engines). These run minimax α-β to depths ≥ 8 plus
    VCF/VCT, way beyond what we need.

[⁸] *Horizon effect* — Wikipedia; Chessprogramming wiki entry. The
    observation that any bounded-depth search can miss tactics that
    materialize just beyond the cutoff. Option C sits at the degenerate
    horizon-zero end of this phenomenon.
