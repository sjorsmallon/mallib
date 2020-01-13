#include "io.h"


io::Platform_Key_Map& io::platform_key_map()
{
	static Platform_Key_Map platform_key_map = {};
	return platform_key_map;
}

io::Keyboard_State& io::keyboard_state()
{
	static Keyboard_State keyboard_state = {};
	return keyboard_state;
}

io::Keyboard_State& io::prev_keyboard_state()
{
	static Keyboard_State previous_keyboard_state = {};
	return previous_keyboard_state;
}
io::Mouse_State& io::mouse_state()
{
	static Mouse_State mouse_state = {};
	return mouse_state;
}

io::Mouse_Coords& io::mouse_coords()
{
	static Mouse_Coords mouse_coords = {};
	return mouse_coords;
}

io::Mouse_Coords& io::prev_mouse_coords()
{
	static Mouse_Coords prev_mouse_coords = {};
	return prev_mouse_coords;
}

std::vector<io::Button>& io::input_queue()
{
	//@Hardcoded
	static std::vector<io::Button> input_queue;
	return input_queue;
}



