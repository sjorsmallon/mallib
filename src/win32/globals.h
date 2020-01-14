#ifndef INCLUDED_GLOBALS_
#define INCLUDED_GLOBALS_
#include <stdint.h>
#include "windows.h" 
// #include "io.h" // Keyboard_State, Mouse_State

struct Globals
{
	int32_t window_width;
	int32_t window_height;
	float aspect_ratio;
	HDC device_context;
	HGLRC gl_context;
};

extern Globals globals;

// keep IO in globals?

#endif
