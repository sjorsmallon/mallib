#ifndef INCLUDED_SHADER_MANAGER_
#define INCLUDED_SHADER_MANAGER_
#include <string>
#include <variant>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>

//--- types -----
using uniform_t = std::variant<glm::mat4, float, int32_t, uint32_t, glm::vec4, glm::ivec2, glm::mat3x4, glm::ivec3, glm::vec3>;

struct Uniform
{
	uint32_t type;
	int32_t location;
	uniform_t data;
	std::string name;
};

struct Shader
{
	uint32_t program_id;
	std::string name;	
	std::map<std::string, Uniform> uniforms;
	// Mostly debug information
	std::vector<std::string> uniform_names;
	std::vector<std::string> attribute_names;
};


struct Shader_Manager
{
	std::map<std::string, Shader> shaders;
	std::string active_shader_name{"none"};
};


void set_shader_path(Shader_Manager& manager, const char* shader_folder_path);

uint32_t  load_shader(Shader_Manager& manager, const std::string& shader_name);
void      set_shader(Shader_Manager& manager, const char* shader_name);
void      set_uniform(Shader_Manager& manager, const std::string& uniform_name, const uniform_t data);



#endif