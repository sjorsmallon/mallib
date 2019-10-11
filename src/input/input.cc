#include "input.h"
#include <vector>

std::vector<input::Key_Input>& input::input_queue()
{
	static std::vector<input::Key_Input> input_queue;
	return input_queue;
}
