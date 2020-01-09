#include "io.h"

io::Keyboard_State& io::keyboard_state()
{
	static Keyboard_State keyboard_state;
	return keyboard_state;
}

io::Keyboard_State& io::previous_keyboard_state()
{
	static Keyboard_State previous_keyboard_state;
	return previous_keyboard_state;
}
io::Mouse_State& io::mouse_state()
{
	static Mouse_State mouse_state;
	return mouse_state;
}


