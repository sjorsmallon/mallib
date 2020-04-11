#ifndef INCLUDED_CLI_ARGUMENTS_
#define INCLUDED_CLI_ARGUMENTS_

namespace cli
{
	struct Arguments
	{
		int window_width;
		int window_height;
		bool create_console;
		int start_x;
		int start_y;

	};
	inline static std::vector<std::string> arguments = {
		"window_width",
		"window_height",
		"create_console",
		"start_x",
		"start_y"
	};

};

#endif