#ifndef INCLUDED_LOG_
#define INCLUDED_LOG_
#include "fmt/core.h"


namespace mlog
{
  void vreport_error(const char *format, fmt::format_args args);

  template <typename... Args>
  void report_error(const char *format, const Args & ... args);

};

void vreport_error(const char *format, fmt::format_args args)
{
  fmt::print("Error: ");
  fmt::vprint(format, args);
}


template <typename... Args>
void mlog::report_error(const char *format, const Args & ... args)
{
   vreport_error(format, fmt::make_format_args(args...));
}

#endif