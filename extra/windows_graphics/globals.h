#ifndef INCLUDED_GLOBALS_
#define INCLUDED_GLOBALS_
#include "io.h" // Keyboard_State, Mouse_State

struct globals
{
	// IO
	Keyboard_State *keyboard_state;
	Keyboard_State *previous_keyboard_sate;
	Mouse_State *mouse_state;

};

globals::init_globals();


#endif
