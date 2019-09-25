#ifndef INCLUDE_GRAPHICS
#define INCLUDE_GRAPHICS_
#include <string>
#include <vector>
#include <stdint.h>
// #include "gl_lite.h"

#ifdef _WIN32
	#include <windows.h>
	// #include <GL/GL.h>
#endif

//@todo: service locator pattern for the  shaders?

namespace graphics
{
	enum class Shader_Type
	{
		SHADER_TEXT,
		SHADER_DEFAULT,
		SHADER_NORMALS,
		SHADER_BOMB
	};

	struct Shaders
	{
		uint32_t bomb_shader_program;
		uint32_t text_shader_program;
		uint32_t normals_shader_program;
		uint32_t default_shader_program;

		// GLuint bomb_shader_program;
		// GLuint text_shader_program;
		// GLuint normals_shader_program;
		// GLuint default_shader_program;
	};

	void init_opengl(); //@Purpose: does that also mean to load all the shaders etc?
	void render_frame();
	void draw_game_3d();
	void clear_buffers();
	void swap_buffers();

	Shaders& shaders();
	void set_shader(Shader_Type shader);
	bool load_compile_attach_shader(uint32_t program, const char* file_name);
	uint32_t shader_type_from_extension(const std::string& filename);

	// text
	void gl_text_mode();
	void draw_text();



	// platform graphics?
	#ifdef _WIN32
	struct Win32_Context
	{
		HGLRC gl_context;
		HDC	device_context;
	};
	Win32_Context& global_Win32_context(); // this is abysmal.
	#endif
	
};


// struct Renderer
// {
//     // list of shaders?
//     // map of shaders?
// };



// class ShaderProgram
// {
//     std::vector<GLuint> shader_IDs;
//     int ID;
// }
// struct Platform_Renderer_Limits
// {
// 	uint32_t max_quads_per_frame;
// };


#endif