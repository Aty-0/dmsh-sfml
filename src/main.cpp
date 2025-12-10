#include "core/game.h"
#include "game/gameSandbox.h"

int main(int, char**)
{
    const auto game = dmsh::core::Game::getInstance();
    game->run<dmsh::game::GameSandbox>();
    //game->run<dmsh::game::EditorSandbox>();
    return 0;
}
