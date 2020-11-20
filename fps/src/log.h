#ifndef INCLUDED_LOG_
#define INCLUDED_LOG_
#include "fmt/core.h"
#include "fmt/color.h"
#include <vector>
#include <string>


namespace logr
{
	void vreport_error(const char* format, fmt::format_args args);

	template<typename... Args>
	void report_error(const char* format, const Args& ...args)
	{
   		vreport_error(format, fmt::make_format_args(args...));
	}

	void vreport_warning(const char* format, fmt::format_args args);

	template<typename... Args>
	void report_warning(const char* format, const Args& ...args)
	{
		vreport_warning(format, fmt::make_format_args(args...));
	}

	void vreport(const char* format, fmt::format_args args);

	template<typename... Args>
	void report(const char* format, const Args& ...args)
	{
		vreport(format, fmt::make_format_args(args...));
	}


	template <typename... Args>
	void debug(const char* format, const Args& ...args)
	{
		vdebug(format, fmt::make_format_args(args...));
	}


	// void vconsole(const char* format, fmt::format_args args);

	template <typename... Args>
	void console(const char* format, const Args& ...args)
	{
		// vconsole(format, fmt::make_format_args(args...));
		logr::console_log().push_back(fmt::format(format, args...));
	}

	std::vector<std::string>& frame_log();
	void clear_frame_log();

	std::vector<std::string>& console_log();
	void clear_console_log();

};

inline void logr::vreport_error(const char* format, fmt::format_args args)
{
  fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "[ERROR]: ");
  fmt::vprint(format, args);
}

inline void logr::vreport_warning(const char* format, fmt::format_args args)
{
	fmt::print(fg(fmt::color::yellow), "[WARNING]: ");
	fmt::vprint(format, args);	
}

inline void logr::vreport(const char* format, fmt::format_args args)
{
	fmt::print(fg(fmt::color::white), "[INFO]: ");
	fmt::vprint(format, args);		
}

// inline void logr::(const char* format, fmt::format_args args)
// {
	// logr::frame_log().push_back(fmt::format(format, args));
// }
// inline void logr::vconsole(const char* format, fmt::format_args args)
// {
// 	logr::console_log().push_back(fmt::format(format, args));
// }

#endif