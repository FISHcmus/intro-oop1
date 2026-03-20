#include "Player.h"

Player::Player(const std::string& name, CellState mark)
    : name(name), mark(mark), wins(0), movesMade(0), nextMove({-1, -1}) {}

Move Player::getMove(const Board& /*board*/) {
    return nextMove;
}

void Player::setNextMove(Move move) {
    nextMove = move;
}

void Player::resetStats() {
    wins = 0;
    movesMade = 0;
}
