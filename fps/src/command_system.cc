#include "command_system.h"
#include "logr.h"
#include <iostream>

namespace
{
	std::map<std::string, Command> f_commands;
}

//@dependencies: f_commands
void add_command(const char* command_name, Command_Function function, Flags flags, const char* description)
{
	// fails if the command already exists.
	if (f_commands.find(command_name) != f_commands.end())
	{
		std::cerr << "[add_command] command already exists!\n";
		// logr::report_warning("[add_command] Command already exists!\n");
		return;
	}
	// otherwise we add the command.

	//@Memory(Sjors): implicit creation
	auto& command = f_commands[command_name];
	command.name = command_name;
	command.function = function;
	command.flags = flags;
	command.description = description;
}

void execute_command(const char* command_name, std::vector<std::string> args)
{
	for (auto& [key, value]: f_commands)
	{
		std::cerr << "command: " << key << '\n';
	}

	if (f_commands.find(command_name) == f_commands.end())
	{
		// logr::report_warning("[execute_command] Command not found!\n");
		std::cerr << "[execute_command] command not found!\n";
		return;
	}

	auto& command = f_commands[command_name];
	command.function(args);
}