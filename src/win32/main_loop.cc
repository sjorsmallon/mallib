#include "game.h"

void game::main_loop(/* float dt? */)
{
    update_game(dt);
    resolve(dt);
    draw(dt);
}


void game::update_game(const float dt)
{
    update_positions();
}

void game::draw(const float dt)
{
    
}