#include <io.h>


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

io::Mouse_State& io::prev_mouse_state()
{
	static Mouse_State prev_mouse_state = {};
	return prev_mouse_state;
}

std::vector<io::Button>& io::input_queue()
{
	static std::vector<io::Button> input_queue;
	return input_queue;
}



