#include "redirect_output.h"
#include <fmt/printf.h>

void redirect::redirect_output()
{
	// Create the console window and set the window title.
	if (AllocConsole() == 0)
	{
		fmt::printf("win32_renderer::redirect_output: allocConsole failed.");
		// Handle error here. Use ::GetLastError() to get the error.
	}

	// Redirect CRT standard input, output and error handles to the console window.
	FILE * pNewStdout = nullptr;
	FILE * pNewStderr = nullptr;
	FILE * pNewStdin = nullptr;

	::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
	::freopen_s(&pNewStdin, "CONIN$", "r", stdin);

	// Clear the error state for all of the C++ standard streams. Attempting to accessing the streams before they refer
	// to a valid target causes the stream to enter an error state. Clearing the error state will fix this problem,
	// which seems to occur in newer version of Visual Studio even when the console has not been read from or written
	// to yet.
	std::cout.clear();
	std::cerr.clear();
	std::cin.clear();

	std::wcout.clear();
	std::wcerr.clear();
	std::wcin.clear();
}
