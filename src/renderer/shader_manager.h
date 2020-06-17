#ifndef INCLUDED_SHADER_MANAGER_
#define INCLUDED_SHADER_MANAGER_
#include <lib/math/mat.h>
#include <mgl/vec2.h>
#include <mgl/vec3.h> // Light
#include <mgl/vec4.h>
#include <mgl/mat4.h>
#include <mgl/mat3.h>


using gl_uniform_t = std::variant<float, int32_t, uint32_t, mgl::ivec2, mgl::vec4, mgl::vec3, mgl::mat4, mgl::mat3>;

struct uniform_t
{
	std::string name;
	int32_t gl_type;
	int32_t gl_index;
	int32_t gl_location;
	gl_uniform_t data;
};

struct attribute_t
{
	std::string name;
	int32_t gl_type;
	uint32_t gl_index;
	uint32_t bit_size;
};

struct shader_info_t
{
	std::map<std::string, uniform_t> uniform_info_t;
	std::map<std::string, attribute_t> attribute_info_t;
};

struct shader_t
{
	uint32_t gl_program_id;
	std::string shader_name;
	shader_info_t info;
};

struct Shader_Manager
{
	// private
	std::map<std::string, uint32_t> shaders; // Owned
	std::string active_shader_name;
	uint32_t active_shader_id;

	Shader_Manager() = default;
	~Shader_Manager() = default;
};


namespace sm
{
	// singleton.
	Shader_Manager& shader_manager();

	// local (static) functions:
	get_gl_shader_info(const Shader_Manager& manager, uint32_t gl_shader_id);
	get_gl_link_success(const Shader_Manager& manager, uint32_t gl_shader_id);
	uint32_t get_shader_type_from_extension(const std::string& filename);

	load_shader_from_path(const std::string& shader_folder_path);
		load_compile_attach_shader(uint32_t gl_shader_id, std::string filename);

	// state management
	set_shader(const Shader_Manager& manager,std::string shader_name);

	update_uniform(const std::string& uniform_name, gl_uniform_t data);
};












#endif

