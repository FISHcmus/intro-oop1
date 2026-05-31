#include "Game.h"
#include "RuntimePaths.h"

int main() {
    RuntimePaths::initialize();

    Game game;
    game.run();
    return 0;
}
