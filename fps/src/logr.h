#ifndef INCLUDED_LOG_
#define INCLUDED_LOG_
#include "fmt/core.h"
#include "fmt/color.h"
#include <vector>
#include <string>
#include <map>

namespace logr
{
	// things we have thought about
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

	// things we still need to think about

	std::map<std::string, std::string>& once_warnings();
	void vreport_warning_once(const char* format, fmt::format_args args);
	template<typename... Args>
	void report_warning_once(const char* format, const Args& ...args)
	{
		vreport_warning_once(format, fmt::make_format_args(args...));
	}


	template <typename... Args>
	void debug(const char* format, const Args& ...args)
	{
		vdebug(format, fmt::make_format_args(args...));
	}


	std::vector<std::string>& console_log();
	void clear_console_log();

	template <typename... Args>
	void console(const char* format, const Args& ...args)
	{
		// vconsole(format, fmt::make_format_args(args...));
		if (logr::console_log().size() > 100)
		{
			logr::console_log().clear();
			logr::report_error("logr::wiped console. make me a ringbuffer please!\n");
		}
		logr::console_log().push_back(fmt::format(format, args...));
	}

	std::vector<std::string>& frame_log();
	void clear_frame_log();


	struct Debug_Variable
	{
		const char* name;
		float* value;
		float min = 0.0f;
		float max = 1.0f;
	};
	std::vector<Debug_Variable>& debug_variables();
	inline void add_float_input(const char* name, float* value, const float min, const float max)
	{
		Debug_Variable debug_variable{name, value, min, max};
		logr::debug_variables().push_back(debug_variable);
	}

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

inline void logr::vreport_warning_once(const char* format, fmt::format_args args)
{
	if (logr::once_warnings().find(format) == logr::once_warnings().end())
	{
		fmt::print(fg(fmt::color::yellow), " [WARNING][ONCE]: ");
		fmt::vprint(format, args);	
		logr::once_warnings()[format] = format;
	}
	
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