#ifndef INCLUDED_IO_
#define INCLUDED_IO_
#include <stdint.h>
#include <vector>
#include <fmt/core.h>

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

	enum class Button : uint16_t
	{
		KEY_W       = 0,
		KEY_A       = 1,
		KEY_S       = 2,
		KEY_D       = 4,
		KEY_UP      = 8,
		KEY_DOWN    = 16,
		KEY_LEFT    = 32,
		KEY_RIGHT   = 64,
		MOUSE_LEFT  = 128,
		MOUSE_RIGHT = 256,
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
		uint8_t lmb_pressed;
		uint8_t rmb_pressed;
		int64_t pos_x;
		int64_t pos_y;
		int64_t pos_delta_x;
		int64_t pos_delta_y;
		double scroll_delta_y;
		double scroll_delta_x;
	};


	Keyboard_State& keyboard_state();
	Keyboard_State& prev_keyboard_state();
	Mouse_State&  mouse_state();
	Mouse_State&  prev_mouse_state();

	// used to map OS keys to internal buttons.
	Platform_Key_Map& platform_key_map();
	
	// button of queues. This could be 
	std::vector<io::Button>& input_queue();

	void update_mouse_coords(const int32_t x, const int32_t y);
	void update_lmb(bool true_if_down);
	void update_rmb(bool true_if_down);
	void update_scroll_delta_y(double scroll_delta_y);


};

inline void io::update_lmb(bool true_if_down)
{
	auto& mouse_state = io::mouse_state();
	mouse_state.lmb_pressed = true_if_down;
}

inline void io::update_rmb(bool true_if_down)
{
	auto& mouse_state = io::mouse_state();
	mouse_state.rmb_pressed = true_if_down;
}


inline void io::update_mouse_coords(const int32_t x, const int32_t y)
{
	auto& mouse_state = io::mouse_state();
	auto& prev_mouse_state = io::prev_mouse_state();

	prev_mouse_state = mouse_state;

	mouse_state.pos_delta_x = x - mouse_state.pos_x;
	mouse_state.pos_delta_y = y - mouse_state.pos_y;
	mouse_state.pos_x = x;
	mouse_state.pos_y = y;
	
}

inline void io::update_scroll_delta_y(double scroll_delta_y)
{
	auto& mouse_state = io::mouse_state();
	mouse_state.scroll_delta_y = scroll_delta_y;
}


#endif