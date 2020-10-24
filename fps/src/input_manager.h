#ifndef INCLUDED_INPUT_MANAGER_
#define INCLUDED_INPUT_MANAGER_
#include <stdint.h>
#include <functional>
#include <map>

using key_code_t = int32_t;
using command_t = std::function<void()>;

struct Input_Manager
{

	std::map<key_code_t, command_t> commands;
	Input_Manager() = default;
	~Input_Manager() = default;
};

void register_command(Input_Manager& manager, key_code_t key_code, command_t command);


#endif

