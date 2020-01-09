#ifndef INCLUDED_GLOBALS_
#define INCLUDED_GLOBALS_
#include "io.h" // Keyboard_State, Mouse_State

struct Globals
{
	// IO
	// Keyboard_State& keyboard_state();
	// Keyboard_State& previous_keyboard_state();
	// Mouse_State&    mouse_state();
	int32_t window_width;
	int32_t window_height;
	float aspect_ratio;
};

extern Globals globals;



#endif
