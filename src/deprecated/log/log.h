#ifndef INCLUDED_LOG_
#define INCLUDED_LOG_
#include "fmt/core.h"


namespace log
{
  enum class Level
  {

  }
  void vreport_error(const char *format, fmt::format_args args);

  template <typename... Args>
  void log_error(const char *format, const Args & ... args);
};

void vreport_error(const char *format, fmt::format_args args)
{
  fmt::print("Error: ");
  fmt::vprint(format, args);
}

template <typename... Args>
void mlog::log_error(const char *format, const Args & ... args)
{
   vreport_error(format, fmt::make_format_args(args...));
}



#endif