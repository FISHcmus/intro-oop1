#include "AIPlayer.h"
#include <algorithm>
#include <chrono>

AIPlayer::AIPlayer(const std::string& name, CellState mark, int searchDepth)
    : Player(name, mark), searchDepth(searchDepth) {}

// Quick heuristic score for move ordering (higher = search first)
int AIPlayer::scoreMove(Board& board, int row, int col, CellState moveMark,
                        CellState aiMark, CellState opponentMark) {
    Move prevLast = board.getLastMove();
    board.placeMove(row, col, moveMark);

    // Instant win/loss detection
    if (board.hasWinner() == moveMark) {
        board.undoMove(row, col, prevLast);
        return (moveMark == aiMark) ? 200000 : 150000;
    }

    // Local evaluation using pattern table around the placed move
    int score = computeLocalScore(board, row, col, aiMark, opponentMark);

    board.undoMove(row, col, prevLast);
    return (score < 0) ? -score : score;  // threats are also important to search
}

Move AIPlayer::getMove(const Board& board) {
    transTable.clear();
    auto candidates = board.getCandidateMoves();
    if (candidates.empty()) {
        return {Board::SIZE / 2, Board::SIZE / 2};
    }
    if (candidates.size() == 1) {
        return candidates[0];
    }

    CellState aiMark = mark;
    CellState opponentMark = (aiMark == CellState::PlayerX)
                                 ? CellState::PlayerO
                                 : CellState::PlayerX;

    // Mutable copy for place/undo during search
    Board searchBoard = board;

    // Score and sort candidates for better alpha-beta pruning
    std::vector<std::pair<int, Move>> scored;
    scored.reserve(candidates.size());
    for (const auto& m : candidates) {
        int s = scoreMove(searchBoard, m.row, m.col, aiMark, aiMark, opponentMark);
        scored.emplace_back(s, m);
    }
    std::sort(scored.begin(), scored.end(),
              [](const std::pair<int, Move>& a, const std::pair<int, Move>& b) {
                  return a.first > b.first;
              });

    // Check for immediate win (highest scored move)
    if (!scored.empty() && scored[0].first >= 200000) {
        lastDebug = {};
        lastDebug.chosenMove = scored[0].second;
        lastDebug.reason = "immediate_win";
        lastDebug.totalCandidates = static_cast<int>(scored.size());
        lastDebug.depthCompleted = 0;
        lastDebug.searchTimeMs = 0;
        lastDebug.topMoves.push_back({scored[0].second, scored[0].first, 200000});
        return scored[0].second;
    }

    // Easy mode (depth 2): direct minimax, no time limit needed
    if (searchDepth <= 2) {
        auto t0 = std::chrono::steady_clock::now();
        int initialScore = evaluate(searchBoard, aiMark, opponentMark);
        int bestScore = -1000000;
        Move bestMove = scored[0].second;
        std::vector<DebugCandidate> allResults;
        for (const auto& pair : scored) {
            const Move& move = pair.second;
            int localBefore = computeLocalScore(searchBoard, move.row, move.col,
                                                aiMark, opponentMark);
            Move prevLast = searchBoard.getLastMove();
            searchBoard.placeMove(move.row, move.col, aiMark);
            int localAfter = computeLocalScore(searchBoard, move.row, move.col,
                                               aiMark, opponentMark);
            int newScore = initialScore - localBefore + localAfter;
            int score = minimax(searchBoard, searchDepth - 1, -1000000, 1000000,
                                false, aiMark, opponentMark, newScore);
            searchBoard.undoMove(move.row, move.col, prevLast);
            allResults.push_back({move, pair.first, score});
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }
        auto t1 = std::chrono::steady_clock::now();
        lastDebug = {};
        lastDebug.chosenMove = bestMove;
        lastDebug.reason = "easy_mode";
        lastDebug.depthCompleted = searchDepth;
        lastDebug.totalCandidates = static_cast<int>(scored.size());
        lastDebug.searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::sort(allResults.begin(), allResults.end(),
                  [](const DebugCandidate& a, const DebugCandidate& b) {
                      return a.searchScore > b.searchScore;
                  });
        for (size_t i = 0; i < std::min(allResults.size(), static_cast<size_t>(5)); i++)
            lastDebug.topMoves.push_back(allResults[i]);
        return bestMove;
    }

    // Medium/Hard: iterative deepening with time limit
    int timeLimitMs = (searchDepth >= 6) ? TIME_LIMIT_HARD_MS : TIME_LIMIT_MEDIUM_MS;
    return iterativeDeepening(searchBoard, scored, aiMark, opponentMark, timeLimitMs);
}

Move AIPlayer::iterativeDeepening(Board& searchBoard,
                                   const std::vector<std::pair<int, Move>>& scored,
                                   CellState aiMark, CellState opponentMark,
                                   int timeLimitMs) {
    auto startTime = std::chrono::steady_clock::now();
    Move bestMove = scored[0].second;
    int completedDepth = 0;
    std::vector<DebugCandidate> lastCompleteResults;

    for (int curDepth = 2; curDepth <= searchDepth; curDepth += 2) {
        transTable.clear();
        int initScore = evaluate(searchBoard, aiMark, opponentMark);
        int bestScore = -1000000;
        Move depthBest = scored[0].second;
        bool timeUp = false;
        std::vector<DebugCandidate> depthResults;

        for (const auto& pair : scored) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime).count();
            if (elapsed >= timeLimitMs) { timeUp = true; break; }

            const Move& move = pair.second;
            int lb = computeLocalScore(searchBoard, move.row, move.col,
                                       aiMark, opponentMark);
            Move prev = searchBoard.getLastMove();
            searchBoard.placeMove(move.row, move.col, aiMark);
            int la = computeLocalScore(searchBoard, move.row, move.col,
                                       aiMark, opponentMark);
            int score = minimax(searchBoard, curDepth - 1, -1000000, 1000000,
                                false, aiMark, opponentMark, initScore - lb + la);
            searchBoard.undoMove(move.row, move.col, prev);

            depthResults.push_back({move, pair.first, score});
            if (score > bestScore) { bestScore = score; depthBest = move; }
        }

        if (!timeUp) {
            bestMove = depthBest;
            completedDepth = curDepth;
            lastCompleteResults = depthResults;
        }

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime).count();
        if (elapsed >= timeLimitMs) break;
    }

    auto endTime = std::chrono::steady_clock::now();
    lastDebug = {};
    lastDebug.chosenMove = bestMove;
    lastDebug.reason = "iterative_deepening";
    lastDebug.depthCompleted = completedDepth;
    lastDebug.totalCandidates = static_cast<int>(scored.size());
    lastDebug.searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime).count();

    std::sort(lastCompleteResults.begin(), lastCompleteResults.end(),
              [](const DebugCandidate& a, const DebugCandidate& b) {
                  return a.searchScore > b.searchScore;
              });
    for (size_t i = 0; i < std::min(lastCompleteResults.size(), static_cast<size_t>(5)); i++)
        lastDebug.topMoves.push_back(lastCompleteResults[i]);

    return bestMove;
}

int AIPlayer::minimax(Board& board, int depth, int alpha, int beta,
                      bool maximizing, CellState aiMark, CellState opponentMark,
                      int boardScore) {
    // Lightweight winner check (no vector allocation)
    CellState winner = board.hasWinner();
    if (winner == aiMark) return 100000 + depth;
    if (winner == opponentMark) return -100000 - depth;
    if (board.isFull()) return 0;
    if (depth <= 0) return boardScore;

    // Transposition table probe
    uint64_t hash = board.getHash();
    auto it = transTable.find(hash);
    if (it != transTable.end()) {
        const TTEntry& entry = it->second;
        if (entry.depth >= depth) {
            if (entry.flag == 0) return entry.score;        // exact
            if (entry.flag == 1 && entry.score > alpha)     // lower bound
                alpha = entry.score;
            if (entry.flag == 2 && entry.score < beta)      // upper bound
                beta = entry.score;
            if (alpha >= beta) return entry.score;
        }
    }

    auto candidates = board.getCandidateMoves();
    if (candidates.empty()) return boardScore;

    // Move ordering: score and sort candidates
    CellState moveMark = maximizing ? aiMark : opponentMark;
    std::vector<std::pair<int, Move>> scored;
    scored.reserve(candidates.size());
    for (const auto& m : candidates) {
        int s = scoreMove(board, m.row, m.col, moveMark, aiMark, opponentMark);
        scored.emplace_back(s, m);
    }
    std::sort(scored.begin(), scored.end(),
              [](const std::pair<int, Move>& a, const std::pair<int, Move>& b) {
                  return a.first > b.first;
              });

    int origAlpha = alpha;
    int bestEval;

    if (maximizing) {
        bestEval = -1000000;
        for (const auto& pair : scored) {
            const Move& move = pair.second;
            int localBefore = computeLocalScore(board, move.row, move.col,
                                                aiMark, opponentMark);
            Move prevLast = board.getLastMove();
            board.placeMove(move.row, move.col, aiMark);
            int localAfter = computeLocalScore(board, move.row, move.col,
                                               aiMark, opponentMark);
            int newScore = boardScore - localBefore + localAfter;
            int eval = minimax(board, depth - 1, alpha, beta, false,
                               aiMark, opponentMark, newScore);
            board.undoMove(move.row, move.col, prevLast);
            if (eval > bestEval) bestEval = eval;
            if (bestEval > alpha) alpha = bestEval;
            if (beta <= alpha) break;
        }
    } else {
        bestEval = 1000000;
        for (const auto& pair : scored) {
            const Move& move = pair.second;
            int localBefore = computeLocalScore(board, move.row, move.col,
                                                aiMark, opponentMark);
            Move prevLast = board.getLastMove();
            board.placeMove(move.row, move.col, opponentMark);
            int localAfter = computeLocalScore(board, move.row, move.col,
                                               aiMark, opponentMark);
            int newScore = boardScore - localBefore + localAfter;
            int eval = minimax(board, depth - 1, alpha, beta, true,
                               aiMark, opponentMark, newScore);
            board.undoMove(move.row, move.col, prevLast);
            if (eval < bestEval) bestEval = eval;
            if (bestEval < beta) beta = bestEval;
            if (beta <= alpha) break;
        }
    }

    // Store in transposition table
    TTEntry newEntry{};
    newEntry.depth = depth;
    newEntry.score = bestEval;
    if (bestEval <= origAlpha) {
        newEntry.flag = 2;  // upper bound (failed low)
    } else if (bestEval >= beta) {
        newEntry.flag = 1;  // lower bound (failed high)
    } else {
        newEntry.flag = 0;  // exact
    }
    transTable[hash] = newEntry;

    return bestEval;
}

int AIPlayer::evaluate(const Board& board, CellState aiMark,
                       CellState opponentMark) {
    if (!patternTableInit) initPatternTable();

    int score = 0;
    static const int DR[] = {0, 1, 1, 1};
    static const int DC[] = {1, 0, 1, -1};

    for (int dir = 0; dir < 4; dir++) {
        int dr = DR[dir];
        int dc = DC[dir];

        // Determine valid starting positions to avoid duplicate scanning
        int rStart = 0, rEnd = Board::SIZE;
        int cStart = 0, cEnd = Board::SIZE;

        // For each direction, we scan all lines by iterating start positions
        // on the "perpendicular" edge
        for (int r = rStart; r < rEnd; r++) {
            for (int c = cStart; c < cEnd; c++) {
                // Only start a line scan from the edge perpendicular to the direction
                bool validStart = false;
                if (dir == 0) validStart = (c == 0);                    // horizontal
                else if (dir == 1) validStart = (r == 0);               // vertical
                else if (dir == 2) validStart = (r == 0 || c == 0);     // diagonal
                else validStart = (r == 0 || c == Board::SIZE - 1);     // anti-diagonal
                if (!validStart) continue;

                // Walk along this line, scanning 5-cell windows
                int lr = r, lc = c;
                int lineLen = 0;
                // Count line length first
                int tr = lr, tc = lc;
                while (tr >= 0 && tr < Board::SIZE && tc >= 0 && tc < Board::SIZE) {
                    lineLen++;
                    tr += dr;
                    tc += dc;
                }

                if (lineLen < 5) continue;

                // Scan windows along this line
                for (int i = 0; i <= lineLen - 5; i++) {
                    int wr = lr + i * dr;
                    int wc = lc + i * dc;
                    int aiIdx = encodeWindow(board, wr, wc, dr, dc, aiMark);
                    int opIdx = encodeWindow(board, wr, wc, dr, dc, opponentMark);
                    // Weight defense higher: opponent threats are 1.2x as important
                    score += patternScore[aiIdx] - (patternScore[opIdx] * 3 / 2);
                }
            }
        }
    }
    return score;
}

// Score contribution of cells near (row, col) along the 4 directional axes.
// Evaluate all 5-cell windows that include (row, col) using pattern table.
int AIPlayer::computeLocalScore(const Board& board, int row, int col,
                                CellState aiMark, CellState opponentMark) {
    if (!patternTableInit) initPatternTable();

    int score = 0;
    static const int DR[] = {0, 1, 1, 1};
    static const int DC[] = {1, 0, 1, -1};

    // Only evaluate 5-cell windows that include (row, col)
    // Defense multiplier: blocking opponent threats is worth more than building your own
    static constexpr int DEF_MULT = 3;
    static constexpr int DEF_DIV = 2;  // effective 1.5x weight on defense

    for (int dir = 0; dir < 4; dir++) {
        int dr = DR[dir];
        int dc = DC[dir];

        // (row,col) can appear at position 0..4 within a 5-cell window
        // Window starts at (row - k*dr, col - k*dc) for k = 0..4
        for (int k = 0; k < 5; k++) {
            int wr = row - k * dr;
            int wc = col - k * dc;

            // Check that all 5 cells of this window are in bounds
            int endR = wr + 4 * dr;
            int endC = wc + 4 * dc;
            if (wr < 0 || wr >= Board::SIZE || wc < 0 || wc >= Board::SIZE) continue;
            if (endR < 0 || endR >= Board::SIZE || endC < 0 || endC >= Board::SIZE) continue;

            int aiIdx = encodeWindow(board, wr, wc, dr, dc, aiMark);
            int opIdx = encodeWindow(board, wr, wc, dr, dc, opponentMark);
            score += patternScore[aiIdx] - (patternScore[opIdx] * DEF_MULT / DEF_DIV);
        }
    }
    return score;
}

// Pattern table: 3^5 = 243 entries
// Encoding: cell 0=empty, 1=self, 2=opponent
// Index = c0 + c1*3 + c2*9 + c3*27 + c4*81
int AIPlayer::patternScore[243] = {};
bool AIPlayer::patternTableInit = false;

void AIPlayer::initPatternTable() {
    for (int idx = 0; idx < 243; idx++) {
        int cells[5];
        int tmp = idx;
        for (int& cell : cells) {
            cell = tmp % 3;
            tmp /= 3;
        }

        // If any opponent piece present, window is blocked for self
        bool hasOpponent = false;
        int selfCount = 0;
        for (int cell : cells) {
            if (cell == 2) { hasOpponent = true; break; }
            if (cell == 1) selfCount++;
        }

        if (hasOpponent || selfCount == 0) {
            patternScore[idx] = 0;
            continue;
        }

        // Score based on count and arrangement
        if (selfCount == 5) {
            patternScore[idx] = 100000;
        } else if (selfCount == 4) {
            // Four in a window = one move from winning
            patternScore[idx] = 15000;
        } else if (selfCount == 3) {
            // Check how consecutive the pieces are
            int maxRun = 0, run = 0;
            for (int cell : cells) {
                if (cell == 1) { run++; if (run > maxRun) maxRun = run; }
                else run = 0;
            }
            if (maxRun == 3) {
                // Consecutive three (e.g., _SSS_, __SSS, SSS__)
                // Check if centered (more open) vs edge
                if (cells[0] == 0 && cells[4] == 0) {
                    patternScore[idx] = 1500;  // _SSS_ — open on both sides in window
                } else {
                    patternScore[idx] = 1000;  // SSS__ or __SSS
                }
            } else if (maxRun == 2) {
                // One gap (e.g., SS_S_, _S_SS, S_SS_)
                patternScore[idx] = 900;
            } else {
                // S_S_S — very spread
                patternScore[idx] = 400;
            }
        } else if (selfCount == 2) {
            int maxRun = 0, run = 0;
            for (int cell : cells) {
                if (cell == 1) { run++; if (run > maxRun) maxRun = run; }
                else run = 0;
            }
            patternScore[idx] = (maxRun == 2) ? 100 : 50;
        } else {
            // Single piece
            patternScore[idx] = 10;
        }
    }
    patternTableInit = true;
}

int AIPlayer::encodeWindow(const Board& board, int r, int c, int dr, int dc,
                           CellState selfMark) {
    int idx = 0;
    int pow3 = 1;
    for (int i = 0; i < 5; i++) {
        int cr = r + i * dr;
        int cc = c + i * dc;
        CellState cell = board.getCell(cr, cc);
        int val = 0;  // empty
        if (cell == selfMark) val = 1;
        else if (cell != CellState::Empty) val = 2;  // opponent
        idx += val * pow3;
        pow3 *= 3;
    }
    return idx;
}



// ---------------------------------------------------------------------------
// Threat-space search
// ---------------------------------------------------------------------------

// Classify the threat level of placing `mark` at (row, col).
// Returns: 0=none, 1=open three, 2=closed four, 3=open four, 4=win
int AIPlayer::classifyThreat(const Board& board, int row, int col, CellState mark) {
    static const int DR[] = {0, 1, 1, 1};
    static const int DC[] = {1, 0, 1, -1};

    int bestLevel = 0;

    for (int d = 0; d < 4; d++) {
        int dr = DR[d];
        int dc = DC[d];

        // Count consecutive pieces in both directions from (row, col)
        // (row, col) is assumed to already hold `mark`
        int fwd = board.countDirection(row, col, dr, dc, mark);
        int bwd = board.countDirection(row, col, -dr, -dc, mark);
        int total = 1 + fwd + bwd;  // including the piece at (row, col)

        if (total >= 5) {
            return 4;  // win — no need to check further
        }

        // Check open ends
        int openEnds = 0;
        int fr = row + dr * (fwd + 1);
        int fc = col + dc * (fwd + 1);
        if (fr >= 0 && fr < Board::SIZE && fc >= 0 && fc < Board::SIZE
            && board.getCell(fr, fc) == CellState::Empty) {
            openEnds++;
        }
        int br = row - dr * (bwd + 1);
        int bc = col - dc * (bwd + 1);
        if (br >= 0 && br < Board::SIZE && bc >= 0 && bc < Board::SIZE
            && board.getCell(br, bc) == CellState::Empty) {
            openEnds++;
        }

        int level = 0;
        if (total == 4 && openEnds == 2) {
            level = 3;  // open four
        } else if (total == 4 && openEnds == 1) {
            level = 2;  // closed four
        } else if (total == 3 && openEnds == 2) {
            level = 1;  // open three
        }

        if (level > bestLevel) {
            bestLevel = level;
        }
    }

    return bestLevel;
}

// Find all moves for `mark` that create a threat (level >= 1).
// Sorted by threat level descending.
std::vector<Move> AIPlayer::findThreats(Board& board, CellState mark) {
    auto candidates = board.getCandidateMoves();
    std::vector<std::pair<int, Move>> threats;

    Move prevLast = board.getLastMove();
    for (const auto& m : candidates) {
        board.placeMove(m.row, m.col, mark);
        int level = classifyThreat(board, m.row, m.col, mark);
        board.undoMove(m.row, m.col, prevLast);

        if (level >= 2) {  // only fours and wins are truly forcing
            threats.emplace_back(level, m);
        }
    }

    std::sort(threats.begin(), threats.end(),
              [](const std::pair<int, Move>& a, const std::pair<int, Move>& b) {
                  return a.first > b.first;
              });

    std::vector<Move> result;
    result.reserve(threats.size());
    for (const auto& t : threats) {
        result.push_back(t.second);
    }
    return result;
}

// Find defensive moves the opponent must play after attacker places at (row, col).
// Returns the cells that block the threat.
std::vector<Move> AIPlayer::findDefenses(Board& board, int row, int col,
                                          CellState attackMark) {
    static const int DR[] = {0, 1, 1, 1};
    static const int DC[] = {1, 0, 1, -1};

    std::vector<Move> defenses;

    for (int d = 0; d < 4; d++) {
        int dr = DR[d];
        int dc = DC[d];

        int fwd = board.countDirection(row, col, dr, dc, attackMark);
        int bwd = board.countDirection(row, col, -dr, -dc, attackMark);
        int total = 1 + fwd + bwd;

        // Check open ends
        int fr = row + dr * (fwd + 1);
        int fc = col + dc * (fwd + 1);
        bool fwdOpen = (fr >= 0 && fr < Board::SIZE && fc >= 0 && fc < Board::SIZE
                        && board.getCell(fr, fc) == CellState::Empty);

        int br = row - dr * (bwd + 1);
        int bc = col - dc * (bwd + 1);
        bool bwdOpen = (br >= 0 && br < Board::SIZE && bc >= 0 && bc < Board::SIZE
                        && board.getCell(br, bc) == CellState::Empty);

        int openEnds = (fwdOpen ? 1 : 0) + (bwdOpen ? 1 : 0);

        // Four in a row — must block the open end(s)
        if (total == 4) {
            if (fwdOpen) defenses.push_back({fr, fc});
            if (bwdOpen) defenses.push_back({br, bc});
        }
    }

    // Deduplicate
    std::sort(defenses.begin(), defenses.end(),
              [](const Move& a, const Move& b) {
                  return a.row < b.row || (a.row == b.row && a.col < b.col);
              });
    defenses.erase(std::unique(defenses.begin(), defenses.end(),
                                [](const Move& a, const Move& b) {
                                    return a.row == b.row && a.col == b.col;
                                }),
                   defenses.end());

    return defenses;
}

// Recursive threat-space search. Returns the first move of a forced winning
// sequence for `attackMark`, or {-1,-1} if none found.
Move AIPlayer::findThreatWin(Board& board, CellState attackMark,
                              CellState defendMark, int depth) {
    if (depth > MAX_THREAT_DEPTH) return {-1, -1};

    auto threats = findThreats(board, attackMark);

    for (const auto& threat : threats) {
        Move prevLast = board.getLastMove();
        board.placeMove(threat.row, threat.col, attackMark);

        // Check for immediate win
        int level = classifyThreat(board, threat.row, threat.col, attackMark);
        if (level == 4) {
            board.undoMove(threat.row, threat.col, prevLast);
            return threat;  // winning move
        }

        // Find forced defensive responses
        auto defenses = findDefenses(board, threat.row, threat.col, attackMark);

        if (defenses.empty()) {
            // No forced defense — this threat isn't actually forcing
            board.undoMove(threat.row, threat.col, prevLast);
            continue;
        }

        bool allDefensesLose = true;
        for (const auto& def : defenses) {
            Move prevLast2 = board.getLastMove();
            board.placeMove(def.row, def.col, defendMark);

            Move result = findThreatWin(board, attackMark, defendMark, depth + 1);

            board.undoMove(def.row, def.col, prevLast2);

            if (result.row < 0) {
                allDefensesLose = false;
                break;
            }
        }

        board.undoMove(threat.row, threat.col, prevLast);

        if (allDefensesLose) {
            return threat;  // forced win found
        }
    }

    return {-1, -1};
}
