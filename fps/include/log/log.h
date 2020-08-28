#ifndef INCLUDED_LOG_
#define INCLUDED_LOG_
#include "fmt/core.h"
#include "fmt/color.h"


namespace logr
{
	template<typename... Args>
	void report_error(const char* format, const Args& ...args)
	{
   		vreport_error(format, fmt::make_format_args(args...));
	}
	void vreport_error(const char* format, fmt::format_args args);
	
	template<typename... Args>
	void report_warning(const char* format, const Args& ...args)
	{
		vreport_warning(format, fmt::make_format_args(args...));
	}
	void vreport_warning(const char* format, fmt::format_args args);


	template<typename... Args>
	void report(const char* format, const Args& ...args)
	{
		vreport(format, fmt::make_format_args(args...));
	}
	void vreport(const char* format, fmt::format_args args);

	// log_error(const char* format, const Args* ...args);
	// ???
	// void print();
	// void print_file(const char* file_name);
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

#endif