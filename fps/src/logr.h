#ifndef INCLUDED_LOG_
#define INCLUDED_LOG_
#include "fmt/core.h"
#include "fmt/color.h"

#include <vector>
#include <string>
#include <map>

// for fmt type print resolution
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
// #include "plane.h"

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

	//@TODO(SMIA): this does not really mesh well with CVAR. think of a way to do this better.
	// struct Debug_Variable
	// {
	// 	const char* name;
	// 	float* value;
	// 	float min = 0.0f;
	// 	float max = 1.0f;
	// };
	// std::vector<Debug_Variable>& debug_variables();
	// inline void add_float_input(const char* name, float* value, const float min, const float max)
	// {
	// 	Debug_Variable debug_variable{name, value, min, max};
	// 	logr::debug_variables().push_back(debug_variable);
	// }

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


// custom formatters.
namespace fmt {

	// glm::vec3
    template <>
    struct formatter<glm::vec3>
    {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const glm::vec3 &lhs, FormatContext &ctx)
		{
			return format_to(ctx.out(), "({:.8f}, {:.8f} {:.8f})",
		   		lhs.x, lhs.y, lhs.z);
		}
    };

    // glm::mat4
    template <>
    struct formatter<glm::mat4>
    {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const glm::mat4 &lhs, FormatContext &ctx)
		{
			return format_to(ctx.out(),
			    "\n r0: |{:.3f} {:.3f} {:.3f} {:.3f}|\n r1: |{:.3f} {:.3f} {:.3f} {:.3f}|\n r2: |{:.3f} {:.3f} {:.3f} {:.3f}|\n r3: |{:.3f} {:.3f} {:.3f} {:.3f}|\n",
			    lhs[0][0], lhs[0][1], lhs[0][2], lhs[0][3],
			    lhs[1][0], lhs[1][1], lhs[1][2], lhs[1][3],
			    lhs[2][0], lhs[2][1], lhs[2][2], lhs[2][3],
			    lhs[3][0], lhs[3][1], lhs[3][2], lhs[3][3]);
      	}
	};


};


// template <>
// struct fmt::formatter<point> {
//   // Presentation format: 'f' - fixed, 'e' - exponential.
//   char presentation = 'f';

//   // Parses format specifications of the form ['f' | 'e'].
//   constexpr auto parse(format_parse_context& ctx) {
//   // auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) // c++11
//     // [ctx.begin(), ctx.end()) is a character range that contains a part of
//     // the format string starting from the format specifications to be parsed,
//     // e.g. in
//     //
//     //   fmt::format("{:f} - point of interest", point{1, 2});
//     //
//     // the range will contain "f} - point of interest". The formatter should
//     // parse specifiers until '}' or the end of the range. In this example
//     // the formatter should parse the 'f' specifier and return an iterator
//     // pointing to '}'.

//     // Parse the presentation format and store it in the formatter:
//     auto it = ctx.begin(), end = ctx.end();
//     if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

//     // Check if reached the end of the range:
//     if (it != end && *it != '}')
//       throw format_error("invalid format");

//     // Return an iterator past the end of the parsed range:
//     return it;
//   }

//   // Formats the point p using the parsed format specification (presentation)
//   // stored in this formatter.
//   template <typename FormatContext>
//   auto format(const point& p, FormatContext& ctx) {
//   // auto format(const point &p, FormatContext &ctx) -> decltype(ctx.out()) // c++11
//     // ctx.out() is an output iterator to write to.
//     return format_to(
//         ctx.out(),
//         presentation == 'f' ? "({:.1f}, {:.1f})" : "({:.1e}, {:.1e})",
//         p.x, p.y);
//   }
// };


#endif