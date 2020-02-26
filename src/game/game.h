#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_
#include <chrono>

namespace game
{
	
	enum class Program_Mode
	{
		MENU,
		GAME
	};

	// //@Refactor: change to double for more precision?
 	std::chrono::duration<double, std::milli>& previous_frame_time();

	void handle_menu_input();
	void simulate_gameplay();

	// initing
	void init();
	void init_audio();
	void init_font();
	void init_graphics();

	// loading
	void load_assets();
	void load_audio();
	void load_shaders();
	void load_models();

	// deiniting
	void deinit_everything();

	void main_loop();
	void shutdown();


	std::map<std::string, uint32_t>& shaders();

};


#endif