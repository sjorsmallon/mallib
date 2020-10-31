#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_
#include <cstdint>

// forward declaration
struct Input;
struct Particle_Cache;


enum Dev_Mode: uint8_t
{
	DM_NONE,
	DM_CAMERA,
};


struct Game_State
{
	Dev_Mode dev_mode = DM_NONE;
	bool paused = false;
	// std::unique_ptr<std::vector<Asset>> assets;

	// Audio_State audio_state;
	// playing_sound* music;

};


void simulate(const float dt, Game_State& game_state,const Input& input, Particle_Cache& particle_cache);


#endif