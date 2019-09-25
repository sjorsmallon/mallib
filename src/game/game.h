#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_


enum class Program_Mode
{
	GAME,
	EDITOR,
	PAUSED,
	MENU
};

namespace game
{

	void load_audio();
	void load_shaders();
	void load_models();

	void init_everything();
	void init_audio();
	void init_font();
	void init_graphics();

	void main_loop();
	void shutdown();

};


#endif