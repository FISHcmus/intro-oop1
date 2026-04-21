# Caro Game AI Algorithm — Implementation Reference

Last updated: 2026-03-22

## Overview

The AI has **two engines** depending on difficulty:

- **Easy/Medium**: Minimax with alpha-beta pruning, pattern table evaluation, transposition table, iterative deepening
- **Hard**: **Rapfi** (GomoCup 2024 champion) — NNUE-based engine running as a subprocess via Gomocup protocol

### Minimax Engine (Easy/Medium)
- Pattern table evaluation (5-cell sliding window)
- Incremental board scoring (avoid full re-evaluation)
- Transposition table (Zobrist hashing)
- Iterative deepening with time limits
- Move ordering heuristic
- Defense-weighted evaluation (1.5x multiplier)
- Threat classification system (used for move ordering, not search)

### Rapfi Engine (Hard)
- **NNUE** (Efficiently Updatable Neural Network) evaluation — Mixnet architecture with pattern codebook distillation
- Alpha-beta search with iterative deepening
- 428,000 nodes/sec on CPU (Mixnet Small)
- Standard rule: exactly 5 in a row wins (overlines don't count)
- Subprocess via Gomocup/Piskvork protocol over stdin/stdout pipes
- Falls back to minimax depth-6 if Rapfi binary is unavailable

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

Player picks one of three labels on the Settings screen; `aiDepth` (2 / 4 / 6) is stored in `settings.cfg` and passed as `AIPlayer(searchDepth = aiDepth)` at construction (`Game.cpp:293`). The branching happens at the top of `getMove()`:

```
AIPlayer::getMove(board)  ─── AIPlayer.cpp:30
  if searchDepth >= 6 && !rapfiFailed   → getRapfiMove()      [Hard]
  ... scoreMove + sort every candidate ...
  if immediate_win (top ≥ 200,000)      → return it           [all levels]
  if searchDepth <= 2                   → fixed-depth block   [Easy]
  else                                  → iterativeDeepening()[Medium / Hard-fallback]
```

### Summary

| Difficulty | `searchDepth` | Primary engine | Primary budget | Fallback |
|---|---|---|---|---|
| Easy        | 2 | Minimax α-β, fixed depth         | none (exhaustive 2-ply)                        | — |
| Medium      | 4 | Iterative deepening minimax α-β  | `TIME_LIMIT_MEDIUM_MS = 1000 ms`               | — |
| Hard (Rapfi)| 6 | Rapfi NNUE subprocess            | `timeoutMs = 10 000 ms` + 2 000 ms pipe slack  | Iterative deepening minimax depth 6, `TIME_LIMIT_HARD_MS = 3000 ms` (only if Rapfi dies; latches via `rapfiFailed`) |

### 1.0 Shared scaffolding (all three levels run this before branching)

```
candidates = board.getCandidateMoves(radius = 2)   ← only empty cells within 2 of any occupied cell
if candidates.empty()      → return center (SIZE/2, SIZE/2)
if candidates.size() == 1  → return the only option

for each candidate m:
    score = scoreMove(m, aiMark, opponentMark)     ← pattern-table heuristic (see §2, §3)
sort candidates desc by score

if top score ≥ 200 000:                            ← "immediate_win" short-circuit
    play it, skip search entirely
```

Pattern table, move-ordering score, and candidate generation are detailed in §2, §3, §6. Sections 1.1–1.3 focus on what happens *after* this sort.

---

### 1.1 Easy — `searchDepth = 2`

**Mechanic:** straight 2-ply minimax α-β with no iterative deepening and no time limit. The AI plays its move (ply 1) and evaluates every single opponent reply (ply 2), then picks the move whose worst-case reply is best.

```
  getMove() — AIPlayer.cpp:80–118
  ───────────────────────────────────────────
  initScore = evaluate(searchBoard, aiMark, opponentMark)   ← full-board pattern sum, once
  bestScore = −∞

  for each (preScore, move) in sorted candidates:
      localBefore = computeLocalScore(move)
      placeMove(move, aiMark)
      localAfter  = computeLocalScore(move)
      newScore    = initScore − localBefore + localAfter    ← incremental eval
      score       = minimax(depth = 1, α = −∞, β = +∞,
                            maximizing = false,
                            boardScore = newScore)
      undoMove
      if score > bestScore: bestScore = score; bestMove = move

  debug.reason = "easy_mode"
  return bestMove
```

| Knob | Value | Source |
|---|---|---|
| `searchDepth` | 2 | `SettingsScreen.cpp:155` (label `"Easy"`) |
| Time budget | none | — |
| Transposition table | cleared per turn at `AIPlayer.cpp:38`, reused inside the turn | — |
| Candidate search radius | 2 | `Board.cpp:135` |

**Feel:** sees only one counter-move deep. Blocks direct fours, misses any fork where the opponent needs two setup moves to win. Beatable with a standard double-three trap.

**Code pointers:** `AIPlayer.cpp:30–44` (setup), `:80–118` (Easy block).

---

### 1.2 Medium — `searchDepth = 4`

**Mechanic:** iterative deepening minimax α-β with a 1 000 ms budget. Tries depth 2 first, then depth 4; only accepts a depth's result if the scan of that depth finished *fully*. If depth 4 times out mid-iteration, the move from the completed depth 2 is returned.

```
  iterativeDeepening() — AIPlayer.cpp:125–190
  ───────────────────────────────────────────
  t0 = now
  for curDepth in [2, 4]:                        ← stride 2
      transTable.clear()                         ← TT is per-depth
      initScore = evaluate(searchBoard)
      bestScore = −∞
      depthBest = scored[0].second
      timeUp    = false

      for each (preScore, move) in scored:
          if now − t0 ≥ 1000 ms:
              timeUp = true; break
          localBefore = computeLocalScore(move)
          placeMove(move, aiMark)
          localAfter  = computeLocalScore(move)
          score = minimax(curDepth − 1, α = −∞, β = +∞, max = false,
                          boardScore = initScore − localBefore + localAfter)
          undoMove
          if score > bestScore: bestScore = score; depthBest = move

      if not timeUp:                             ← commit only complete depths
          bestMove       = depthBest
          completedDepth = curDepth
      if now − t0 ≥ 1000 ms: break

  debug.reason = "iterative_deepening"
```

`minimax()` itself does move-ordered α-β (`AIPlayer.cpp:220–272`), probes the transposition table (`:203–215`), and stores results with `flag ∈ {0 = exact, 1 = lower bound, 2 = upper bound}` (`:274–285`).

| Knob | Value | Source |
|---|---|---|
| `searchDepth` | 4 | `SettingsScreen.cpp:156` |
| Time budget | 1 000 ms | `AIPlayer.h:59` (`TIME_LIMIT_MEDIUM_MS`) |
| Depth schedule | 2 → 4 | `AIPlayer.cpp:134` |
| TT | cleared between depths, reused within a depth | `:135`, `:285` |
| Incremental eval | `boardScore − localBefore + localAfter` | `:154` |

**Feel:** catches open threes, blocks open fours, sees your depth-2 threat sequences. Still falls to forks that need depth 6+ to resolve (e.g., a buried VCF win).

**Code pointers:** `AIPlayer.cpp:120–190` (ID driver), `:192–288` (minimax + TT).

---

### 1.3 Hard — `searchDepth = 6` → Rapfi subprocess

**Mechanic:** delegates the whole decision to **Rapfi**, the GomoCup 2024 champion NNUE engine, via the Gomocup stdin/stdout protocol. Rapfi runs as a forked child process, `chdir()`'d to `assets/rapfi/` so it picks up `config.toml` and the `mix9svqstandard_bs15.bin.lz4` weights. On any failure (process crash, timeout, invalid move, pipe error), `rapfiFailed` latches `true` for the rest of the session and the AI silently degrades to a depth-6 iterative deepening minimax with a 3 s budget.

```
  getMove() — AIPlayer.cpp:32–36
  ──────────────────────────────
  if searchDepth ≥ 6 && !rapfiFailed:
      m = getRapfiMove(board)
      if m.row ≥ 0: return m             ← success
      ... otherwise fall through to scoreMove + iterativeDeepening ...

  getRapfiMove() — AIPlayer.cpp:667–729
  ─────────────────────────────────────
  if !rapfiEngine:
      rapfiEngine = new RapfiEngine("assets/rapfi", timeoutMs = 10000)
      if !rapfiEngine->start():          ← fork + execl("./rapfi") + handshake
          rapfiFailed = true; return {-1,-1}
      boardSynced = false; lastSentMoveCount = 0

  if moveCount == 0:               → BEGIN              ← engine plays first
  elif !boardSynced || mismatch:   → BOARD … DONE       ← full resync (after undo / load)
  else:                            → TURN col,row        ← incremental (note col,row, not row,col)

  parse "x,y" → Move{row = y, col = x}
  if target cell not empty:  rapfiFailed = true; destroy engine; return {-1,-1}
  debug.reason = "rapfi_engine"
```

Rapfi lifecycle (`RapfiEngine.cpp`):

```
start():  pipe() + fork()
          child : dup2 stdin/stdout, chdir(binaryDir), execl("./rapfi")
          parent: initialize() sends  START 15
                                      INFO rule 1
                                      INFO timeout_turn 10000
                                      INFO timeout_match 0
                                      INFO PONDERING 1
readMove(): poll()-based line reader, skips MESSAGE / DEBUG / ERROR lines
stop():   write "END", 50 ms grace, close pipes, SIGTERM, 200 ms, SIGKILL, waitpid
          (prevents orphan processes — commit 6d211fd)
```

| Knob | Value | Source |
|---|---|---|
| `searchDepth` gate | ≥ 6 | `AIPlayer.cpp:32` |
| Engine-side per-move budget | 10 000 ms (`timeout_turn`) | `RapfiEngine.cpp` init, `assets/rapfi/config.toml` |
| Parent-side pipe timeout | `timeoutMs + 2 000 ms = 12 000 ms` | `RapfiEngine.cpp:213`, `:230`, `:259` |
| Rule | `INFO rule 1` (standard — exactly 5 wins, overlines don't count) | — |
| Pondering | `INFO PONDERING 1` (think during opponent's turn) | — |
| Weights | `mix9svqstandard_bs15.bin.lz4` (Mixnet small, 15×15 standard) | `assets/rapfi/` |
| TT size | 128 MB | `config.toml` → `default_tt_size_kb = 131072` |
| Threads | all cores | `default_thread_num = 0` |
| Fallback algorithm | iterative deepening minimax to depth 6 | `AIPlayer.cpp:120–122` |
| Fallback budget | 3 000 ms | `AIPlayer.h:60` (`TIME_LIMIT_HARD_MS`) |

**Feel:** tournament-strength. First move of a game adds ~1 s for weight loading. Rapfi's own time manager typically returns in 1–3 s on obvious positions and stretches to ~7.5 s (75 % of 10 s via `advanced_stop_ratio`) only when the evaluation is volatile. Most human players don't win without a handicap.

**Failure modes handled:**
- Rapfi binary missing → `start()` fails → `rapfiFailed = true` → next move uses minimax.
- Rapfi hangs → parent pipe timeout fires → fallback, engine destroyed.
- User loads a save or undoes → `resetEngine()` flips `boardSynced = false` without killing the process → next `getMove()` issues `BOARD … DONE` to resync.
- Rapfi returns an occupied cell (protocol violation) → engine destroyed, fallback.

**Code pointers:** `AIPlayer.cpp:32–36` (gate), `:667–729` (wrapper), `:731–734` (`resetEngine`); `RapfiEngine.cpp` for the subprocess and protocol; `assets/rapfi/config.toml` for engine tuning.

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

## 8. Rapfi Engine Integration

### Architecture

```
AIPlayer::getMove()
  ├── searchDepth >= 6 && !rapfiFailed?
  │   └── getRapfiMove()
  │       ├── Lazy init: create RapfiEngine, fork/exec rapfi subprocess
  │       ├── boardSynced? → TURN x,y (incremental)
  │       ├── !boardSynced? → BOARD ... DONE (full resync)
  │       ├── moveCount == 0? → BEGIN (engine plays first)
  │       └── Failure? → rapfiFailed = true, fall through to minimax
  └── Minimax (fallback or Easy/Medium)
```

### Subprocess Management (`RapfiEngine` class)

- **Lifecycle**: `fork()` + `dup2()` + `execl("./rapfi")`. Child process `chdir()`s to `assets/rapfi/` so Rapfi finds `config.toml` and weight files.
- **I/O**: Pipes with `poll()`-based timeout reads. Skips `MESSAGE`/`DEBUG`/`ERROR` lines from Rapfi output.
- **Coordinate conversion**: Gomocup uses `(x,y)` = `(col,row)`. Game uses `(row,col)`.
- **Board resync**: After undo or save load, `resetEngine()` sets `boardSynced=false`. Next `getMove()` sends full board state via `BOARD` command.
- **Fallback**: Any failure (process crash, pipe error, timeout, invalid move) sets `rapfiFailed=true` and falls through to minimax.

### Gomocup Protocol Commands Used

| Command | When | Format |
|---|---|---|
| `START 15` | Engine init | Initializes empty 15x15 board |
| `INFO rule 1` | After START | Standard rule (exactly 5 wins) |
| `INFO timeout_turn 10000` | After START | 10s max per move |
| `INFO timeout_match 0` | After START | No match budget (turn-only mode) |
| `INFO PONDERING 1` | After START | Think during opponent's turn |
| `BEGIN` | AI plays first | Engine returns its first move |
| `TURN x,y` | Opponent moved | Engine returns its response |
| `BOARD ... DONE` | Resync | Send full board, engine responds |
| `END` | Game over | Terminate engine process |

### Rapfi Config (`assets/rapfi/config.toml`)

| Setting | Value | Purpose |
|---|---|---|
| `default_thread_num` | 0 | Use all CPU cores |
| `default_tt_size_kb` | 131072 | 128 MB transposition table |
| `message_mode` | none | No debug output (faster pipe I/O) |
| `advanced_stop_ratio` | 0.75 | GomocalC interactive setting — exits early on obvious moves |
| `type` (evaluator) | mix9svq | NNUE Mixnet architecture |
| Weight file | `mix9svqstandard_bs15.bin.lz4` | Standard rule, 15x15 board |

### Time Management

Rapfi uses smart time allocation in "turn only" mode (`timeout_match 0`):
- **Obvious positions** (stable best move, no eval drops): 1-3 seconds
- **Complex positions** (best move changing, eval dropping): up to ~7.5s (75% of 10s)
- **Forced moves** (only one non-losing option): <1 second
- **Mate found**: stops after 24 more iterations, typically <1s
- **Pondering**: thinks during human's turn, building TT for faster subsequent search

---

## 9. Debug System

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

## 10. Known Weaknesses

### Easy/Medium (Minimax)

1. **No forced-win detection**: Minimax with depth 4-6 can't see wins/threats beyond ~6 plies. A proper VCF/VCT implementation would fix this but previous attempt was buggy.

2. **Defense multiplier is a band-aid**: The 1.5x defense weight helps but doesn't solve fundamental horizon problems. The AI can still miss threats beyond its search depth.

3. **No candidate pruning**: All ~30-50 candidates are searched at every node. Pruning to 12-15 would allow 2-3 plies deeper search in the same time budget.

4. **Transposition table cleared between depths**: Keeping it across depths (with proper depth checks) would speed up iterative deepening.

5. **Move ordering at internal nodes is expensive**: `scoreMove()` calls `computeLocalScore()` which does 20 window lookups. A cheaper ordering (e.g., killer moves, history heuristic) would help.

### Hard (Rapfi)

1. **GPL v3 binary**: Rapfi is GPL — bundling as subprocess is commonly accepted but legally gray. CC0 weights are fine.

2. **~12 MB added to game size**: Binary (1.8 MB) + weights (9.9 MB) + classical model (23 KB).

3. **First move startup**: Weight loading takes ~1s on first move of a game. Subsequent moves are faster due to pondering.

---

## 11. Change History

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
| 2026-03-22 | Board 19x19 → 15x15 | Kept |
| 2026-03-22 | Rapfi engine for Hard mode | Kept |
| 2026-03-22 | Settings persistence (settings.cfg) | Kept |
