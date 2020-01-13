#ifndef INCLUDED_IO_
#define INCLUDED_IO_
#include <stdint.h>
#include <vector>

// What this is:

// What it is not:

namespace io
{
	// for when vec2 is not defined.
	struct Mouse_Coords
	{
		int x;
		int y;
	};

	enum class Button : uint8_t
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
		bool is_pressed[256];

		// This is int for windows specific reasons.
		bool& operator[](int idx)
		{
			return is_pressed[idx];
		}

	};

	struct Platform_Key_Map
	{
		io::Button data[256];

		io::Button& operator[](int idx)
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

	Platform_Key_Map& platform_key_map();


	Keyboard_State& keyboard_state();
	Keyboard_State& prev_keyboard_state();

	Mouse_State&  mouse_state();
	Mouse_Coords& mouse_coords();
	Mouse_Coords& prev_mouse_coords();

	std::vector<io::Button>& input_queue();

	void update_mouse_coords(const int32_t x, const int32_t y);

};

inline void io::update_mouse_coords(const int32_t x, const int32_t y)
{
	io::prev_mouse_coords().x = io::mouse_coords().x;
	io::prev_mouse_coords().y = io::mouse_coords().y;

	io::mouse_coords().x = x;
	io::mouse_coords().y = y;
}


#endif