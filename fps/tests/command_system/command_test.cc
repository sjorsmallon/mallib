#include "../../src/command_system.h"
#include <iostream>


// example command
inline void cmd_set_position(const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		// logr::report_warning("too few arguments. provide 3 floats.\n");
		std::cerr << "too few arguments. provide 3 floats.\n";
		return;
	}
	// logr::report("executing set_position");
	std::cerr << "executing set_position\n";
}

int main()
{
	add_command("set_position", cmd_set_position, CMD_FL_CHEAT, "set camera position\n");

	

	execute_command("set_position", {"12.0f", "12.0f", "12.0f"});

}