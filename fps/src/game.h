#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_


enum Dev_Mode: uint8
{
	DM_NONE,
	DM_CAMERA,
}


struct Game_State
{
	Dev_Mode dev_mode = DM_NONE;
	// std::unique_ptr<std::vector<Asset>> assets;

	// Audio_State audio_state;
	// playing_sound* music;

}


void simulate(const float dt, Game_State& game_state, Input input, Particle_Cache& particle_cache);


#endif