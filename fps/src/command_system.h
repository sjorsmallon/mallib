#ifndef INCLUDED_COMMAND_SYSTEM_
#define INCLUDED_COMMAND_SYSTEM_
#include <vector>
#include <functional>
#include <string>

#include "logr.h"

using Command_Function = std::function<void(const std::vector<std::string>&)>;
using Flags = uint64_t;


enum Command_Flags {
	CMD_FL_ALL				= 0,
	CMD_FL_CHEAT			= 1 << 0,	// command is considered a cheat
	CMD_FL_SYSTEM			= 1 << 1,	// system command
	CMD_FL_RENDERER			= 1 << 2,	// renderer command
	CMD_FL_SOUND			= 1 << 3,	// sound command
	CMD_FL_GAME				= 1 << 4,	// game command
	CMD_FL_TOOL				= 1 << 5	// tool command
};

struct Command
{
	std::string name;
	Command_Function function;
	Flags flags;
	std::string description;
};

void add_command(const char* command_name, Command_Function function, Flags flags, const char* description);


void execute_command(const char* command_name, std::vector<std::string> args);




#endif // INCLUDED_COMMAND_SYTEM_