#ifndef INCLUDE_GRAPHICS
#define INCLUDE_GRAPHICS_
#include <string>
#include <vector>

#ifdef _WIN32
	#include <windows.h>
	#include <GL/GL.h>
#endif=

struct Platform_Renderer_Limits
{
	uint32_t max_quads_per_frame;
};

struct Platform_Graphics
{
	// Renderer_Texture_Queue texture_queue;
};

namespace graphics
{
	void render_frame();
	void swap_buffers();

	bool load_and_compile_shader(Shader& shader, const std::string& filename);
	GLenum graphics::shader_type_from_extension(const std::string& filename);

	// platform graphics?
	#ifdef _WIN32
	struct Win32_Context
	{
		HGLRC gl_context;
		HDC	device_context;
	};
	Win32_Context& global_Win32_context();
	#endif


	
};

// namespace window; // i think

// namespace renderer
// {
//     GLenum shader_type_from_extension(const std::string& filename);
    
//     void init_opengl();
// }


// struct Renderer
// {
//     // list of shaders?
//     // map of shaders?
// };

// struct Shader
// {
//     GLuint shader_ID; 
//     GLenum shader_type; // resolve shader type from filename
// };

// class ShaderProgram
// {
//     std::vector<GLuint> shader_IDs;
//     int ID;
// }



#endif