#include "input_manager.h"

void register_command(Input_Manager& manager, key_code_t key_code, command_t command)
{
	manager.commands[key_code] = command;
}
