#ifndef INCLUDED_RENDER_MANAGER_
#define INCLUDED_RENDER_MANAGER_
#include <windows.h>
#include "camera.h"

//--- OpenGL
#include <gl_lite/gl_lite.h>

// forward declaration : renderer/camera.h


struct Win32_Context
{
	HGLRC gl_context;
	HDC device_context;
	HWND window_handle;
};

// enum Render_Mode
// {
// 	2D_LH_D,
// 	3D_LH_PERSPECTIVE,
// 	3D_LH_ORTHOGRAPHIC,
// 	3D_LH_FPS,
// }

struct Render_Manager
{
	cam::Camera  active_camera;
	// scene::Scene active_scene;
	Win32_Context win32_context;

};


namespace rm
{
	Render_Manager& render_manager();

	void init();
		void init_render_manager();
		void init_opengl();
		void init_imgui(Render_Manager& manager);
		void init_cvars();

	// direct gl interface
	void clear_buffer_bits();
	void swap_buffers(Render_Manager& render_manager); // requires context.



	// // camera 
	// void render_2d_left_handed_dc(const uint32_t active_shader);
	// void render_3d_left_handed_perspective(const uint32_t active_shader);


	//--- rendering 
	// void render_frame();
		void render_ui();
	// 	void render_centroid_axes();
	// 	void render_game_3d();
	// 	void clear_buffer_bits();
	// 	void swap_buffers();
};


#endif