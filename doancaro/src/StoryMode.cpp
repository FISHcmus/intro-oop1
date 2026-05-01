#include "StoryMode.h"
#include "StoryContent.h"

namespace StoryMode {

State::State() { reset(); }

void State::reset() {
    subBeat = SubBeat::IntroMonologue;
    introPageIdx = 0;
    currentSet = SetId::Set1;
    matchWinsInSet = 0;
    matchLossesInSet = 0;
    matchesPlayedInSet = 0;
    for (auto& orb : matchOutcomes) orb = OrbState::Pending;
    voiCharges = -1;
    gaCharges = -1;
    nguaCharges = -1;
    gaActiveTurns = 0;
    bossPlayerMoveCounter = 0;
}

int State::getCurrentDifficulty() const {
    switch (currentSet) {
        case SetId::Set1:      return 1;
        case SetId::Set2:      return 2;
        case SetId::Set3:      return 3;
        case SetId::FinalBoss: return 4;
    }
    return 1;
}

int State::matchesToWin() const {
    // Best-of-3 in EVERY set including FinalBoss. The narrative beats the
    // sigil orbs visualize use this same threshold (2 wins or 2 losses).
    return 2;
}

bool State::isLastSetMatch() const {
    int needed = matchesToWin();
    return matchWinsInSet == needed - 1 || matchLossesInSet == needed - 1;
}

bool State::isUnlocked(LinhVat lv) const {
    return chargesLeft(lv) >= 0;
}

int State::chargesLeft(LinhVat lv) const {
    switch (lv) {
        case LinhVat::Voi:  return voiCharges;
        case LinhVat::Ga:   return gaCharges;
        case LinhVat::Ngua: return nguaCharges;
    }
    return -1;
}

void State::advance() {
    switch (subBeat) {
        case SubBeat::IntroMonologue:
            if (introPageIdx + 1 < StoryContent::kIntroPageCount) {
                ++introPageIdx;
            } else {
                subBeat = SubBeat::SetIntro;
            }
            break;

        case SubBeat::SetIntro:
            // Caller transitions GameState to Playing on this subBeat change.
            // Reset all set-level state so the sigil starts at three Pending
            // orbs and the boss cheat clock starts fresh. Linh vật charges
            // refresh per the current set — same hand whether you got here
            // via linear advance, lose-replay, or picker jump.
            matchWinsInSet = 0;
            matchLossesInSet = 0;
            matchesPlayedInSet = 0;
            for (auto& orb : matchOutcomes) orb = OrbState::Pending;
            bossPlayerMoveCounter = 0;
            refreshLinhVatForCurrentSet();
            subBeat = SubBeat::MatchPlaying;
            break;

        case SubBeat::SetWin:
            // Set1/2/3 wins → unlock screen. FinalBoss win → epilogue.
            if (currentSet == SetId::FinalBoss) {
                subBeat = SubBeat::Epilogue;
            } else {
                subBeat = SubBeat::LinhVatUnlock;
            }
            break;

        case SubBeat::LinhVatUnlock:
            // Bump to next set. Charge grants happen uniformly when the
            // SetIntro→MatchPlaying transition runs refreshLinhVatForCurrentSet.
            switch (currentSet) {
                case SetId::Set1: currentSet = SetId::Set2;      break;
                case SetId::Set2: currentSet = SetId::Set3;      break;
                case SetId::Set3: currentSet = SetId::FinalBoss; break;
                case SetId::FinalBoss: break;  // unreachable: boss has no unlock
            }
            subBeat = SubBeat::SetIntro;
            break;

        case SubBeat::SetLose:
            // Replay the same set from its intro.
            subBeat = SubBeat::SetIntro;
            break;

        case SubBeat::Epilogue:
            // Caller transitions to Menu.
            break;

        case SubBeat::MatchPlaying:
            // advance() not meaningful while a match is live.
            break;
    }
}

void State::onMatchStart() {
    // Per-match transient state. Set-level counters survive between
    // best-of-3 matches; only ephemeral effects clear here.
    bossPlayerMoveCounter = 0;
    gaActiveTurns = 0;
}

bool State::onMatchEnd(bool playerWon) {
    // Record the chronological outcome for the StorySigil before tallying,
    // so the orb index lines up with matchesPlayedInSet (0,1,2).
    if (matchesPlayedInSet < 3) {
        matchOutcomes[matchesPlayedInSet] =
            playerWon ? OrbState::Won : OrbState::Lost;
        ++matchesPlayedInSet;
    }

    int needed = matchesToWin();
    if (playerWon) ++matchWinsInSet;
    else           ++matchLossesInSet;

    if (matchWinsInSet >= needed) {
        subBeat = SubBeat::SetWin;
        return true;
    }
    if (matchLossesInSet >= needed) {
        subBeat = SubBeat::SetLose;
        return true;
    }
    return false;
}

bool State::useVoi() {
    if (voiCharges <= 0) return false;
    --voiCharges;
    return true;
}

bool State::useGa() {
    if (gaCharges <= 0) return false;
    --gaCharges;
    gaActiveTurns = 1;  // one AI turn plays random
    return true;
}

bool State::tryUseNguaOnLoss() {
    // Caller invokes this BEFORE recording the loss via onMatchEnd. On
    // success the caller skips onMatchEnd entirely — the match never
    // counts. Charge is one-shot for the whole run.
    if (nguaCharges <= 0) return false;
    --nguaCharges;
    return true;
}

bool State::consumeGaTurn() {
    if (gaActiveTurns <= 0) return false;
    --gaActiveTurns;
    return true;
}

bool State::tickBossCheat() {
    if (currentSet != SetId::FinalBoss) return false;
    ++bossPlayerMoveCounter;
    if (bossPlayerMoveCounter >= 4) {
        bossPlayerMoveCounter = 0;
        return true;
    }
    return false;
}

void State::refreshLinhVatForCurrentSet() {
    voiCharges  = (currentSet >= SetId::Set2)      ? 1 : -1;
    gaCharges   = (currentSet >= SetId::Set3)      ? 3 : -1;
    nguaCharges = (currentSet == SetId::FinalBoss) ? 1 : -1;
}

void State::jumpToSet(SetId target) {
    reset();
    currentSet = target;
}

void State::restore(SetId setId, int wins, int losses,
                    int voi, int ga, int ngua) {
    reset();

    if (wins   < 0) wins   = 0;
    if (losses < 0) losses = 0;
    if (wins   > kMatchesPerSet) wins   = kMatchesPerSet;
    if (losses > kMatchesPerSet) losses = kMatchesPerSet;
    int played = wins + losses;
    if (played > kMatchesPerSet) played = kMatchesPerSet;

    subBeat            = SubBeat::MatchPlaying;
    currentSet         = setId;
    matchWinsInSet     = wins;
    matchLossesInSet   = losses;
    matchesPlayedInSet = played;

    for (int i = 0; i < kMatchesPerSet; ++i) {
        if (i < wins)        matchOutcomes[i] = OrbState::Won;
        else if (i < played) matchOutcomes[i] = OrbState::Lost;
        else                 matchOutcomes[i] = OrbState::Pending;
    }

    voiCharges  = voi;
    gaCharges   = ga;
    nguaCharges = ngua;
}

}  // namespace StoryMode
