#ifndef INCLUDE_GRAPHICS
#define INCLUDE_GRAPHICS_
#include <string>
#include <stdint.h>
#include <map> // textures, buffers, shaders.

#ifdef _WIN32
	#include <windows.h>
	// #include <GL/GL.h>
#endif

#include "../asset/asset.h"
#include "../scene/scene.h"
#include "../vec3/vec3.h" // Light

/*
 *  init_graphics()
 * init_opengl()
 *  render_nd_whatever();
 */

namespace graphics
{
	//@Incomplete: these draw modes / VBO&VAO management should move to the manager.
	struct Buffers
	{
		uint32_t VAO;
		uint32_t VBO;
		uint32_t IBO;
	};

	//--- init. ---------------------------
	void init_graphics();
	void init_opengl();
	void init_texture_settings(std::map<std::string, asset::Texture>& textures);
	
	//--- openGL helpers. ---------------------------
	void get_shader_info(uint32_t shader_program);
	bool get_link_success(uint32_t shader_program);

	//--- openGL bookkeeping. ---------------------------
	std::map<std::string, uint32_t>& shaders();
	uint32_t next_free_texture_frame();
	scene::Scene& active_scene();
	std::map<std::string, graphics::Buffers>& buffers();

	//--- shader helpers. ---------------------------
	void     set_shader(const std::string& shader_name);
	uint32_t load_shader(const std::string& shader_folder_path);
	uint32_t load_compile_attach_shader(uint32_t program, std::string file_name);
	void     reload_shaders(uint32_t& program);
	uint32_t shader_type_from_extension(const std::string& filename);

	//--- font/text mode. ---------------------------
	void gl_text_mode();
	void draw_text();

	//--- rendering ----------------------------------
	void render_frame();
	void draw_game_3d();
	void clear_buffer_bits();
	void swap_buffers();

	//--- drawing modes -------------------------------
	void render_2d_left_handed_dc(const uint32_t active_shader);
	void render_3d_left_handed_perspective(const uint32_t active_shader);
	// void draw_2d_left_handed_orthographic();
	// void draw_2d_left_handed_normalized();






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

#endif