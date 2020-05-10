#ifndef INCLUDE_GRAPHICS
#define INCLUDE_GRAPHICS_
#include <map> // textures, buffers, shaders.
#include <string>
#include <stdint.h>
#include <variant>
#include <vector>

#ifdef _WIN32
	#include <windows.h>
#endif

// forward declarations.
#include <asset.h>
#include <scene.h>
#include <io.h> // for mouse_input 

#include <mgl/vec2.h>
#include <mgl/vec3.h> // Light
#include <mgl/vec4.h>
#include <mgl/mat4.h>
#include <mgl/mat3.h>
#include <camera.h>

using uniform_type = std::variant<float, int32_t, uint32_t, mgl::ivec2, mgl::vec4, mgl::vec3, mgl::mat4, mgl::mat3>;

namespace graphics
{
	struct Uniform
	{
		int32_t type;
		std::string name;
		uniform_type data;
		int32_t location;
	};

	struct Shader
	{
		uint32_t program_id;
		std::string shader_name;
		std::vector<std::string> uniform_names;
		std::vector<std::string> attribute_names;	
		std::map<std::string, Uniform> uniforms;
	};

	struct Buffers
	{
		uint32_t VAO;
		uint32_t VBO;
		uint32_t IBO;
	};

	//--- init
	void init_graphics();
		void init_opengl();
		void init_imgui();
		void init_texture_settings(std::map<std::string, asset::Texture>& textures);
		void init_cvars();

	//--- active elements.
	scene::Scene& active_scene();
	cam::Camera&  active_camera();
	std::string&  active_shader_name();
	uint32_t& 	  active_shader_id();

	std::map<std::string, graphics::Shader>& shader_info();
	std::map<std::string, uint32_t>& shaders();
	std::map<std::string, graphics::Buffers>& buffers();

	//--- textures
	uint32_t next_free_active_texture_ID();

	//--- shader helpers
	void     set_shader(const std::string& shader_name);
	uint32_t load_shader(const std::string& shader_folder_path);
	uint32_t load_compile_attach_shader(uint32_t program, std::string file_name);
	void 	 get_shader_info(graphics::Shader& shader);
	bool 	 shader_link_succeeded(uint32_t shader_program);
	uint32_t shader_type_from_extension(const std::string& filename);
	

	//--- update (can / ought to be called per-frame)
	void update_uniform(const std::string& uniform_name, uniform_type data);
	void update_active_camera(io::Mouse_State& mouse_state); // camera occupies view & perspective matrices.



	//--- font/text mode
	void gl_text_mode();
	void draw_text();

	//--- rendering 
	void render_frame();
		void render_ui();
		void render_centroid_axes();
		void render_game_3d();
		void clear_buffer_bits();
		void swap_buffers();

	//--- drawing modes
	void render_2d_left_handed_dc(const uint32_t active_shader);
	void render_3d_left_handed_perspective(const uint32_t active_shader);

	// platform graphics?
	#ifdef _WIN32
	struct Win32_Context
	{
		HGLRC gl_context;
		HDC	device_context;
		HWND window_handle;
	};
	Win32_Context& global_Win32_context(); // this is abysmal.
	#endif
	
};

#endif