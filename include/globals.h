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
	//double previous_frame_time;
	HDC device_context;
	HGLRC gl_context;
	HWND window_handle;
};

extern Globals globals;

// keep IO in globals?


// struct Window_Settings
// {
// 	uint32_t width;
// 	uint32_t height;
// 	uint32_t start_x;
// 	uint32_t start_y;
// 	bool allow_vsync;
//     bool allow_fullscreen;
//     std::string title;
// };

// struct Globals
// {
// 	Window_Settings window_settings;


// };

#endif
