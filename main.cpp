#include "Game.h"

int main()
{
    Game game;
    if(!game.load())
        return 1;
    game.run();
    return 0;
}
