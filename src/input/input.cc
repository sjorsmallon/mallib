#include "input.h"
#include <vector>

std::vector<input::Key_Input>& input::input_queue()
{
	static std::vector<input::Key_Input> input_queue;
	return input_queue;
}

Vec2i& input::new_mouse_coordinates()
{
	static Vec2i new_mouse_coordinates;
	return new_mouse_coordinates;
}
