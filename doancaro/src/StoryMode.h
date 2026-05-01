#pragma once

// Story Mode state machine. Pure state — no rendering, no input. The Game
// class drives advance() / onMatchEnd() / useXxx() calls; the screen funcs
// inspect this state to decide what to draw.
//
// Best-of-3 in EVERY set (Set1/Set2/Set3/FinalBoss). First to 2 wins or
// 2 losses decides. The 3rd-orb slot stays Pending if a 2-0 sweeps the set.

namespace StoryMode {

// Best-of-3 within every set. Bound shared by matchOutcomes[] and the
// reconstruction loop in State::restore.
static constexpr int kMatchesPerSet = 3;

enum class SetId { Set1 = 0, Set2 = 1, Set3 = 2, FinalBoss = 3 };

enum class SubBeat {
    IntroMonologue,   // showing intro page introPageIdx
    SetIntro,         // showing currentSet's intro line
    MatchPlaying,     // game is in GameState::Playing — no story panel rendered
    SetWin,           // showing currentSet's win line
    SetLose,          // showing currentSet's lose line
    LinhVatUnlock,    // showing the linh vật line for currentSet
    Epilogue,         // post-FinalBoss-win closing line
};

enum class LinhVat { Voi = 0, Ga = 1, Ngua = 2 };

// Per-match outcome record for the StorySigil. Fills chronologically:
// matchOutcomes[0] = first match in this set, [1] = second, [2] = third.
enum class OrbState { Pending, Won, Lost };

class State {
public:
    State();
    void reset();

    // Narrative state
    SubBeat subBeat;
    int     introPageIdx;
    SetId   currentSet;

    // Best-of-3 within currentSet
    int matchWinsInSet;
    int matchLossesInSet;

    // Chronological per-match outcomes for the StorySigil. Index 0 = first
    // match played in this set. Reset on every set entry.
    OrbState matchOutcomes[kMatchesPerSet];
    int      matchesPlayedInSet;   // how many slots in matchOutcomes are filled

    // Linh vật charges. -1 = locked (not yet unlocked). >=0 = remaining uses.
    int voiCharges;
    int gaCharges;
    int nguaCharges;

    // Gà active counter — while > 0, AI plays a random move on its next turn.
    int gaActiveTurns;

    // FinalBoss "cheat" tick — counts player moves; every 4 moves boss
    // removes pieces. Reset on FinalBoss entry.
    int bossPlayerMoveCounter;

    int getCurrentDifficulty() const;
    bool isLastSetMatch() const;            // true when on match-point (next decides)
    bool isUnlocked(LinhVat lv) const;
    int  chargesLeft(LinhVat lv) const;

    // Advance from a non-MatchPlaying subBeat to the next state.
    // Caller (Game) transitions GameState afterwards based on new subBeat.
    void advance();

    // Picker entry: jump to any set's intro narrative regardless of where
    // the state machine was. Resets all set-level + per-match state and
    // points subBeat at IntroMonologue (so the global monologue still plays
    // first). Caller transitions GameState to StoryIntro.
    void jumpToSet(SetId target);

    // Resume a saved Story Mode match mid-set. Sets subBeat to MatchPlaying
    // (skips narration), restores per-set counters and linh-vật charges,
    // and packs matchOutcomes[] in win-first order. Win-first packing is
    // an approximation — exact match chronology is not persisted.
    void restore(SetId setId, int wins, int losses,
                 int voi, int ga, int ngua);

    // Called by Game right before each new match in a set (including the
    // first). Resets per-match transient state (boss cheat counter, ga turns).
    // Does NOT reset set-level counters — those live with onSetStart logic
    // inside advance()'s SetIntro case.
    void onMatchStart();

    // Called by Game when a match completes. Updates wins/losses counters
    // AND records the outcome in matchOutcomes[matchesPlayedInSet].
    // Returns true if the set is now decided (caller should route to
    // StoryBeat with the new SetWin/SetLose subBeat); returns false when
    // another match in the set is needed (caller should startNewGame again).
    bool onMatchEnd(bool playerWon);

    // Linh vật actions. Each returns true on success (charge consumed),
    // false if locked / no charges.
    bool useVoi();    // caller does the actual undo×5 on the Board
    bool useGa();     // sets gaActiveTurns = 1 (one AI turn random)
    // Ngựa is automatic on match loss — see tryUseNguaOnLoss().
    bool tryUseNguaOnLoss();   // returns true if ngua saved the player

    // AI side: call before each AI turn. Returns true (and decrements the
    // ga counter) if the AI should play a random move this turn.
    bool consumeGaTurn();

    // FinalBoss only: call after each player move. Returns true exactly
    // once every 4th call — that's when the boss should rip 4 pieces.
    bool tickBossCheat();

private:
    int matchesToWin() const;   // 2 for Set1-3, 1 for FinalBoss

    // Refill charges based on currentSet. Called on every SetIntro entry,
    // so jumps (via picker / cheat) and replays (after a SetLose) all give
    // the same per-set starting hand. Set 1 = nothing, Set 2 = Voi, Set 3 =
    // Voi+Gà, FinalBoss = all three. Charges reset to full each set entry.
    void refreshLinhVatForCurrentSet();
};

}  // namespace StoryMode
