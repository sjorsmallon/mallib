#ifndef INCLUDED_SHADER_PROGRAM_
#define INCLUDED_SHADER_PROGRAM_

// stl
#include <string>
#include <map>
#include <vector>
#include <stdint.h>
#include <variant>

#include <mgl/mat4.h>
#include <mgl/mat3.h>
#include <mgl/vec4.h>
#include <mgl/vec3.h>
#include <mgl/vec2.h>

// for now, only supports GL shaders.

using uniform_type = std::variant<float, int32_t, uint32_t, mgl::ivec2, mgl::vec4, mgl::vec3, mgl::mat4, mgl::mat3>;


namespace gl
{

	struct Uniform
	{
		int32_t gl_type;
		std::string name;
		uniform_type data;
		int32_t gl_location;
	};

	struct Shader_Info
	{
		std::vector<std::string> uniform_names;
		std::vector<std::string> attribute_names;
	};

	struct Shader_Program
	{
		uint32_t program_id;
		std::string name;
		std::map<std::string, Uniform> uniforms;		
		Shader_Info info;
	};

	struct Shader_Manager()
	{
		std::map<std::string, Shader_Program> shader_programs;
		uint32_t active_shader_id; // private?

	};
	set_shader(const Shader_Manager& shader_manager, const std::string& shader_name);
	load_shader(const Shader_Manager& shader_manager, const std::string& shader_folder_path);
	get_shader_info_gl(const Shader_Manager& shader_manager, gl::Shader_Program& shader_program); // local function, could be static.
	get_shader_link_success_gl(const Shader_Manager& shader_manager, uint32_t shader_id);
	update_uniform(const Shader_Manager& shader_manager, uint32_t shader_id, const std::string& uniform_name, uniform_type data);
};







#endif