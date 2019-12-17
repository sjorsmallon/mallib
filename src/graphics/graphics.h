#ifndef INCLUDE_GRAPHICS
#define INCLUDE_GRAPHICS_
#include <string>
#include <vector> // load_obj_to_vertex
#include <stdint.h>
// #include "gl_lite.h"

#ifdef _WIN32
	#include <windows.h>
	// #include <GL/GL.h>
#endif

#include "../asset/asset.h"
#include "../scene/scene.h"
#include "../vec3/vec3.h" // Light

#include <map> // for VAO/VBO stuff. will move.


namespace graphics
{
	// struct Render_Settings
	// {
	// 	bool render_isophotes;
	// 	bool render_normals;
	// 	bool 
	// 	bool
	// 	bool
	// 	bool
	// }

	struct Light
	{
		Vec3 position;
		Vec3 light_color;
		// what kind of light is this?
	};

	enum class Shader_Type
	{
		SHADER_TEXT,
		SHADER_DEFAULT,
		SHADER_NORMALS,
		SHADER_GOURAUD,
		SHADER_ISOPHOTES,
		SHADER_REFLECTION_LINES,
		SHADER_BOMB
	};

	struct Shaders //@Note: shader_programs?
	{
		uint32_t bomb;
		uint32_t text;
		uint32_t normals;
		uint32_t gouraud;
		uint32_t isophotes;
		uint32_t reflection_lines;
		uint32_t default;
	};

	struct Window_Settings
	{
		float width;
		float height;
		bool v_sync;
	};


	void init_graphics(); //@Note: does that also mean to load all the shaders etc?
	void init_opengl();
	void setup_shaders();
	void get_shader_info(uint32_t shader_program);

	void render_frame();
	void draw_game_3d();
	void clear_buffers();
	void swap_buffers();
	

	Shaders& shaders();
	Window_Settings& window_settings();
	

	//@Refactor what does this return? shader_id, program, id?
	// using program_id, shader_id = uint32_t;
	uint32_t load_compile_attach_shader(uint32_t program, std::string file_name);
	uint32_t shader_type_from_extension(const std::string& filename);

	// font/text mode.
	void gl_text_mode();
	void draw_text();
	void reload_shaders(uint32_t& program);

	// active_shader?
	void set_shader(Shader_Type shader);
	// uint32_t& active_shader();


	//@temporary
	scene::Scene& active_scene();
	void init_texture_settings(std::map<std::string, asset::Texture>& textures);

	//@Incomplete: these draw modes / VBO&VAO management should move to the manager.
	struct Buffers
	{
		uint32_t VAO;
		uint32_t VBO;
		uint32_t IBO;
	};

	std::map<std::string, graphics::Buffers>& buffers();

	// draw modes.
	// void draw_2d_right_handed_normalized()
	// void draw_3d_right_hand_perspective();

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