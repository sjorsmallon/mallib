#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_
#include <chrono>

namespace game
{

	enum class Program_Mode
	{
		GAME,
		EDITOR,
		PAUSED,
		MENU
	};
	//@Refactor: move to globals.
	Program_Mode& global_program_mode();
	//@Refactor: change to double for more precision?
    std::chrono::duration<float, std::milli>& previous_frame_time();

	void load_audio();
	void load_shaders();
	void load_models();


	void simulate_gameplay(); // make this static in the class?
	//@Refactor: is input handling like this better? or keep it
	// centralized in one function which then evaluates what the game state is
	void handle_menu_input();



	// initing
	void init_everything();
	void init_audio();
	void init_font();
	void init_graphics();

	// loading
	void load_everything();

	// deiniting
	void deinit_everything();

	void main_loop();
	void shutdown();

};


#endif