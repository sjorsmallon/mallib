#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_
#include <chrono>
#include <map>

namespace game
{
	
	enum class Program_Mode
	{
		MENU,
		GAME,
		DEBUG
	};

 	std::chrono::duration<double, std::milli>& previous_frame_time();

	// void handle_menu_input();
	// void handle_debug_input();
	// void simulate_gameplay();

	// initing
	void init();
		// void init_audio();
		// void init_font();
		// void init_graphics();
		// void init_cvars();

	// loading
		// void load_assets();
		// void load_audio();
		// void load_shaders();
		// void load_models();

	// deiniting
	void deinit_everything();

	void main_loop();
	void shutdown();


	std::map<std::string, uint32_t>& shaders();

};


#endif