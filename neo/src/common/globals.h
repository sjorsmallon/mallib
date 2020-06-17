#ifndef INCLUDED_GLOBALS_
#define INCLUDED_GLOBALS_
#include <stdint.h>
#include "windows.h" 
// #include "io.h" // Keyboard_State, Mouse_State

struct Win32_Globals
{
	HDC device_context;
	HGLRC gl_context;
	HWND window_handle;
};

extern Win32_Globals win32_globals;


#endif
