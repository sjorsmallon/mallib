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

	void init_everything();
	void init_audio();
	void init_font();
	void init_graphics();

	void deinit_everything();

	void main_loop();
	void shutdown();

};


#endif