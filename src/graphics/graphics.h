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

#include "../vec3/vec3.h" // Vertex
#include "../vec2/vec2.h" // Vertex
//@todo: service locator pattern for the shaders?

namespace graphics
{
	//@Cleanup: we can sort of remove this. or keep it internal facing.
	struct Face
	{
		Vec3u indices_set[3]; // set of indices for a vertex.
	};
	
	struct Vertex
	{
		Vec3 position;
		Vec3 normals;
		Vec2 tex_coords;
	};


	struct Raw_Obj_Data
	{
		std::vector<Vec3> positions;
		std::vector<Vec3> normals;
		std::vector<Vec2> tex_coords;
		std::vector<Face> faces; // these indices are offset by 1.
		std::vector<Vertex> vertices;
	};

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
		SHADER_BOMB
	};

	struct Window_Settings
	{
		float width;
		float height;
		bool v_sync;
	};

	struct Shaders //@Note: shader_programs?
	{
		uint32_t bomb;
		uint32_t text;
		uint32_t normals;
		uint32_t default;
	};

	void init_graphics(); //@Note: does that also mean to load all the shaders etc?
	void render_frame();
	void draw_game_3d();
	void clear_buffers();
	void swap_buffers();
	void get_shader_info(uint32_t program);

	Shaders& shaders();
	Window_Settings& window_settings();

	
	void set_shader(Shader_Type shader);
	uint32_t load_compile_attach_shader(uint32_t program, std::string file_name);
	uint32_t shader_type_from_extension(const std::string& filename);

	//@Refactor: this name.
	void load_obj(const std::string& filename, Raw_Obj_Data& raw_data);
    void generate_vertices_from_raw_data(Raw_Obj_Data& raw_data); // either this should return something, but I want this to do something.

	// text
	void gl_text_mode();
	void draw_text();
	void reload_shaders(uint32_t& program);

	//@temporary
	graphics::Raw_Obj_Data& cat_data();



	// draw modes.
	// void draw_2d_right_handed_normalized()


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