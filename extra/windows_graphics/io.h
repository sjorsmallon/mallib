#ifndef INCLUDED_IO_
#define INCLUDED_IO_
#include <stdint.h>

namespace io
{
	enum Button
	{
		KEY_W,
		KEY_A,
		KEY_S,
		KEY_D,
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		MOUSE_LEFT,
		MOUSE_RIGHT,
	};

	struct Keyboard_State 
	{
		uint8_t data[256];

		uint8_t& operator[](size_t idx)
		{
			return data[idx];
		}

	};

	struct Mouse_State
	{
		uint8_t lmb;
		uint8_t rmb;
		float scroll_up;
		float scroll_down;
	};

	Keyboard_State& keyboard_state();
	Keyboard_State& previous_keyboard_state();
	Keyboard_State& platform_key_map();
	Mouse_State&    mouse_state();

};



#endif