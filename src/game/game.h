#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_
#include <chrono>


namespace game
{


	// //@Refactor: change to double for more precision?
 	// std::chrono::duration<float, std::milli>& previous_frame_time();


	//@Refactor: is input handling like this better? or keep it
	// centralized in one function which then evaluates what the game state is
	void handle_menu_input();
	void simulate_gameplay(); // make this static in the class?


	// initing
	void init_everything();
	void init_audio();
	void init_font();
	void init_graphics();

	// loading
	void load_everything();
	void load_audio();
	void load_shaders();
	void load_models();

	// deiniting
	void deinit_everything();

	void main_loop();
	void shutdown();

};


#endif