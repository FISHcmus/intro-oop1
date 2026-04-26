#pragma once

#include <cstdint>
#include <vector>

void initJudge(int n, bool isCycle, int treasure,
               const std::vector<bool>& initialLights, std::uint64_t seed);

void startBob(int bobStart);

std::vector<int> getInitialObservation();

int getCurRoom();
int getAliceFlipsUsed();
const std::vector<int>& getAliceFlipPositions();
std::vector<bool> getLightsSnapshot();
int getMoveCount();

void setMoveLimit(int limit);  // 0 = no enforcement
