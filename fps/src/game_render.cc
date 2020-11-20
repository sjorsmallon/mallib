#include "game.h"
#include "render_system.h"

void game_render(Game_State& game_state, Particle_Cache& particle_cache)
{
	render(game_state.camera, particle_cache);
	//render_shadows(game_state.camera, particle_cache);
}