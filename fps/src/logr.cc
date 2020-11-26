#include "log.h"

std::vector<std::string>& logr::frame_log()
{
	static std::vector<std::string> frame_log;

	return frame_log;
}

std::vector<std::string>& logr::console_log()
{
	static std::vector<std::string> console_log;

	return console_log;
}

std::map<std::string, std::string>& logr::once_warnings()
{
	static std::map<std::string, std::string> once_warnings;

	return once_warnings;
}

void logr::clear_frame_log()
{
	logr::frame_log().clear();
}

void logr::clear_console_log()
{
	logr::frame_log().clear();
}