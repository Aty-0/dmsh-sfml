#include "core/game.h"

int main(int, char**)
{
    const auto game = dmsh::core::Game::getInstance();
    game->run();
    return 0;
}
