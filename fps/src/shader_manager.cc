#include "shader_manager.h"
#include <string>
#include <vector>

#include <glad/glad.h>
#include "logr.h"
#include <glm/gtc/type_ptr.hpp>
#include "file.h"

// Defines
constexpr const int NONE_SHADER_PROGRAM_ID = 0;

namespace 
{
    std::string g_shader_folder_prefix = "../assets/shaders/";
    Shader_Manager* g_shader_manager = nullptr;


    bool get_shader_link_success(uint32_t program_id)
    {
        int gl_link_status = 0;
        glGetProgramiv(program_id, GL_LINK_STATUS, &gl_link_status);
        return gl_link_status;
    }

    GLenum shader_type_from_extension(const std::string filename)
    {
        std::string_view view(filename);
        view = view.substr(view.find_last_of(".") + 1);

        if (view == "vert")
            return GL_VERTEX_SHADER;
        else if (view == "frag")
            return GL_FRAGMENT_SHADER;
        else if (view == "geo")
            return GL_GEOMETRY_SHADER;
        else if (view == "tesc")
            return GL_TESS_CONTROL_SHADER;
        else if (view == "tese")
            return GL_TESS_EVALUATION_SHADER;
        else if (view == "comp")
            return GL_COMPUTE_SHADER;
        else
            return 0;
    }

    uint32_t load_compile_attach_shader(uint32_t program_id,const std::string &file_name)
    {
        constexpr const int GL_FAILURE = 0;
        const std::string filename = file_name;
        const GLenum shader_type = shader_type_from_extension(filename);

        if (shader_type == GL_FAILURE) logr::report_error("[graphics] incorrect shader type. filename: {}\n", filename);

        const uint32_t shader_id = glCreateShader(shader_type);

        if (shader_id == GL_FAILURE) logr::report_error("[graphics] glCreateShader failed.\n");
        else 
        {
            // logr::report("[graphics] glCreateShader succeeded. created shader ID {} \n",shader_id);
        }

        std::string target = {};

        file_to_string(filename, target);
        const char* shader_c_str = target.c_str();
        glShaderSource(shader_id, 1, &shader_c_str, NULL);
        glCompileShader(shader_id);

        GLint max_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);
        std::vector<GLchar> info_log(max_length);        
		if (info_log.size() > 0)
		{
			glGetShaderInfoLog(shader_id, max_length, &max_length, &info_log[0]);
            std::string string_log = std::string(info_log.begin(), info_log.end());
            logr::report("[graphics] shader info log:\n {} \n", string_log);
		}


        GLint shader_compiled = GL_FALSE; 
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);

        if (shader_compiled != GL_TRUE)
        {
            logr::report_error("[graphics] shader_from_file: unable to compile shader {} \n", filename);
            glDeleteShader(shader_id); // Don't leak the shader.
            exit(1);
        }
        else
        {
            logr::report("[graphics] shader_from_file: successfully compiled {}\n", filename);
            glAttachShader(program_id, shader_id);
        }

        return shader_id;
    }


    void get_shader_info(Shader_Manager& shader_manager, Shader& shader)
    {
        bool print_shader_info = false;

       if (print_shader_info) logr::report("[graphics] shader info for program {} \n", shader.name);
        set_shader(shader_manager, shader.name.c_str());

        std::vector<GLchar> name_data(256);
        std::vector<GLenum> properties = {};
        properties.push_back(GL_NAME_LENGTH);
        properties.push_back(GL_TYPE);
        properties.push_back(GL_ARRAY_SIZE);
        std::vector<GLint> values(properties.size());

        //--- Attributes
        GLint numActiveAttribs = 0;
        glGetProgramInterfaceiv(shader.program_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);

        for(size_t attrib = 0; attrib < numActiveAttribs; ++attrib)
        {
            glGetProgramResourceiv(shader.program_id, GL_PROGRAM_INPUT, attrib, properties.size(),
            &properties[0], values.size(), nullptr, &values[0]);
            name_data.resize(values[0]);
            glGetProgramResourceName(shader.program_id, GL_PROGRAM_INPUT, attrib, name_data.size(), NULL, &name_data[0]);
            //IC(Sjors): -1 here is to skip the null character (I think.)
            std::string attribute_name(name_data.begin(), name_data.end() - 1);
            shader.attribute_names.push_back(attribute_name);
        }

        //--- Uniforms
        GLint active_uniform_count = 0;
        glGetProgramInterfaceiv(shader.program_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_uniform_count);
        for(size_t uniform_idx = 0; uniform_idx != active_uniform_count; ++uniform_idx)
        {
            glGetProgramResourceiv(shader.program_id, GL_UNIFORM, uniform_idx, properties.size(),
            &properties[0], values.size(), nullptr, &values[0]);
            name_data.resize(values[0]);

            glGetProgramResourceName(shader.program_id, GL_UNIFORM, uniform_idx, name_data.size(), NULL, &name_data[0]);
            //IC(Sjors): -1 here is to skip the null character.
            std::string uniform_name(name_data.begin(), name_data.end() - 1);//
            shader.uniform_names.push_back(uniform_name);
        }
        //--- Uniform types
        std::vector<uint32_t> uniform_indices(shader.uniform_names.size());
        std::vector<const char*> string_pointers;
        std::vector<int32_t> uniform_types(shader.uniform_names.size());

        for (auto& uniform_name: shader.uniform_names)
        {
            string_pointers.push_back(uniform_name.c_str());
        }
        assert(shader.uniform_names.size() == string_pointers.size());
        assert(uniform_indices.size() == shader.uniform_names.size());
        
        // get indices and their names.
        glGetUniformIndices(shader.program_id, shader.uniform_names.size(), string_pointers.data(), uniform_indices.data());
        glGetActiveUniformsiv(shader.program_id, shader.uniform_names.size(), uniform_indices.data(),  GL_UNIFORM_TYPE, uniform_types.data());

        for (size_t idx = 0; idx != shader.uniform_names.size(); ++idx)
        {
            auto uniform_type = uniform_types[idx];
            switch(uniform_type)
            {
                case GL_FLOAT_VEC4:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT_VEC4;
                    uniform.data = glm::vec4(0.0f);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_FLOAT_VEC4\n", uniform.name);
                    break;
                }
                case GL_FLOAT_VEC3:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT_VEC3;
                    uniform.data = glm::vec3(0.0f);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_FLOAT_VEC3\n", uniform.name);
                    break;
                }
                case GL_FLOAT:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT;
                    uniform.data = float{0.0f};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_FLOAT\n", uniform.name);
                    break;
                }
                case GL_FLOAT_MAT4:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT_MAT4;
                    uniform.data = glm::mat4(1.0f);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_FLOAT_MAT4\n", uniform.name);
                    break;
                }
                case GL_FLOAT_MAT3x4:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT_MAT3x4;
                    uniform.data = glm::mat3x4(1.0f);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_FLOAT_MAT3x4\n", uniform.name);
                    break;
                }

                case GL_INT:
                {
                    Uniform uniform;
                    uniform.type = GL_INT;
                    uniform.data = int32_t{0};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_INT\n", uniform.name);
                    break;
                }
                case GL_INT_VEC2:
                {
                    Uniform uniform;
                    uniform.type = GL_INT_VEC2;
                    uniform.data = glm::ivec2(0);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_INT_VEC2\n", uniform.name);
                    break;
                }
                case GL_INT_VEC3:
                {
                    Uniform uniform;
                    uniform.type = GL_INT_VEC3;
                    uniform.data = glm::ivec3(0);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_INT_VEC3\n", uniform.name);

                    break;
                }
                case GL_SAMPLER_2D:
                {
                    Uniform uniform;
                    uniform.type = GL_SAMPLER_2D;
                    uniform.data = int32_t{0};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_SAMPLER_2D\n", uniform.name);
                    break;
                }
                case GL_UNSIGNED_INT:
                {
                    Uniform uniform;
                    uniform.type = GL_UNSIGNED_INT;
                    uniform.data = uint32_t{0};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_UNSIGNED_INT\n", uniform.name);
                    break;
                }
                case GL_UNSIGNED_INT_ATOMIC_COUNTER:
                {
                    // Uniform uniform;
                    // uniform.type = GL_UNSIGNED_INT;
                    // uniform.data = uint32_t{0};
                    // uniform.name = shader.uniform_names[idx];
                    // shader.uniforms[uniform.name] = uniform;
                   if (print_shader_info) logr::report("{} : GL_UNSIGNED_INT_ATOMIC_COUNTER\n", shader.uniform_names[idx]);
                    break;
                }
                default:
                {
                    logr::report_error("[Uniform] {}  : UNKNOWN TYPE", shader.uniform_names[idx]);
                    break;
                }
            }
        }

        
        logr::report_warning("[get_shader_info] skipping glGetUniformLocation for uniform arrays.\n");

        constexpr const int INVALID_UNIFORM_LOCATION = -1;
        for (auto &uniform_name: shader.uniform_names)
        {
            //@FIXME(Sjors): dirty fix to not check for location of array uniforms, since they will be set using 
            // named buffer data.
            if (uniform_name.find("[") != std::string::npos) continue;


            shader.uniforms[uniform_name].location = glGetUniformLocation(shader.program_id, uniform_name.c_str());
            if (shader.uniforms[uniform_name].location == INVALID_UNIFORM_LOCATION) logr::report_error("[Uniform] {} : UNKNOWN LOCATION!\n", uniform_name);
        }

        glUseProgram(0);

    }
}

void set_global_shader_manager(Shader_Manager& shader_manager)
{
    g_shader_manager = &shader_manager;
}

//@dependencies: g_shader_manager.
void shader_reload_callback(const std::string& shader_folder_name)
{
        // get the shader manager.
        Shader_Manager& manager = get_global_shader_manager();
        std::string shader_name = shader_folder_name.substr(shader_folder_name.find_last_of('/') + 1);
        logr::report("Detected change. reloading shader: {}\n", shader_name);
        bool shader_found = manager.shaders.find(shader_name) != manager.shaders.end();
        
        assert(shader_found && "shader not found!");

        if (shader_found)
        {
            // clear shader info, except for name.
            clear_shader_gl_components(manager, shader_name);
            // at this point, the shader no longer exists, and we can (re)load the shader.
            load_shader(manager, shader_name);
        }
}

//@assumptions: shader_name is a valid shader name.
void clear_shader_gl_components(Shader_Manager& shader_manager, const std::string& shader_name)
{
    auto& shader = shader_manager.shaders[shader_name];
    auto old_shader_id = shader.program_id;
    shader.program_id = NONE_SHADER_PROGRAM_ID;
    shader.uniforms.clear();
    shader.uniform_names.clear();
    shader.attribute_names.clear();

    // also delete the shader.
    //@VOLATILE(Sjors): the shaders are now deleted after linking in load_compile_attach_shader();
    glDeleteProgram(old_shader_id);
}



Shader_Manager& get_global_shader_manager()
{
    return *g_shader_manager;
}

void set_shader_path(Shader_Manager& manager, const char* shader_folder_path)
{
    g_shader_folder_prefix = shader_folder_path;
}


//@IC(Sjors):the "none" shader is not any shader.
void set_shader(Shader_Manager& shader_manager, const char* shader_name)
{
    uint32_t shader_id = NONE_SHADER_PROGRAM_ID; 
    // if (shader_name != "none")
    {
        shader_id = shader_manager.shaders[shader_name].program_id;
    }
    shader_manager.active_shader_name = shader_name;

    //@Fixme(Sjors): strcmp..
    if (shader_id == 0 && strcmp(shader_name,"none") != 0)
    {
        logr::report_error("[set_shader]: UNKNOWN SHADER NAME: {}. exiting.", shader_name);
        exit(1);
    }

    glUseProgram(shader_id);
}
 
uint32_t load_shader(Shader_Manager& shader_manager, const std::string& shader_name)
{
    std::string shader_folder_path{};

    if (shader_manager.shaders.find(shader_name) != shader_manager.shaders.end())
    {
        logr::report_warning("[load_shader] loading a shader we have already loaded! ");
        exit(1);
    }

    //@FIXME(Sjors): this is a very hacky way to detect whether we were called from "main" or as a reload.
    if (shader_name[0] == '.')
    {
        shader_folder_path = shader_name;
    }
    else
    {
        shader_folder_path = g_shader_folder_prefix + shader_name;
    }

    const uint32_t shader_program = glCreateProgram();

    std::vector<uint32_t> shader_ids;
    if (!std::filesystem::exists(shader_folder_path)) logr::report_error("[graphics] shader folder {} does not exist!\n",shader_folder_path);

    for (const auto& file: std::filesystem::directory_iterator(shader_folder_path))
        shader_ids.push_back(load_compile_attach_shader(shader_program, file.path().string()));

    glLinkProgram(shader_program);

    if (!get_shader_link_success(shader_program)) logr::report_error("[graphics] error: shader {} could not be linked!\n", shader_folder_path);
    
    // Detach and delete shaders in any case. we do not need them after the shader program is linked.
    for (const auto& shader_id: shader_ids)
    {
        glDetachShader(shader_program, shader_id);
        glDeleteShader(shader_id);
    }
    
    // gather uniforms & attributes.
    // @FIXME(Sjors): implicit creation!
    Shader& shader = shader_manager.shaders[shader_name];
    shader.name = shader_name;
    shader.program_id =  shader_program;

    set_shader(shader_manager, shader_name.c_str());
    get_shader_info(shader_manager, shader);
    
    set_shader(shader_manager, "none");
    return shader_program;
}

//--- this function assumes the target shader is bound before uniforms are updated, and will break 
// and warn the user if that is not the case.
void set_uniform(Shader_Manager& shader_manager, const std::string& uniform_name, const uniform_t data)
{
    auto& active_shader = shader_manager.shaders[shader_manager.active_shader_name];
    if (active_shader.uniforms.find(uniform_name) == active_shader.uniforms.end())
    {
        logr::report_warning("[OpenGL] uniform {} does not exist in shader {}. ignoring.\n", uniform_name, shader_manager.active_shader_name);
        return;
    }

    auto& uniform = active_shader.uniforms[uniform_name];

    std::visit([&](auto&& uniform_data, auto&& new_data)
    {
        using T = std::decay_t<decltype(uniform_data)>;
        using my_T = std::decay_t<decltype(new_data)>;
        if constexpr(!(std::is_same_v<T, my_T>))
        {
            logr::report_error("[Shader_Manager] set_uniform: NAME: {} type mismatch. \n", uniform_name);
        }

        if constexpr (std::is_same_v<T, glm::vec3>)
        {
            uniform.data = data;
            glUniform3fv(uniform.location, 1, glm::value_ptr(std::get<glm::vec3>(data)));
        }

        if constexpr (std::is_same_v<T, glm::vec4>)
        {
            uniform.data = data;
            glUniform4fv(uniform.location, 1, glm::value_ptr(std::get<glm::vec4>(data)));
        }
        else if constexpr (std::is_same_v<T, glm::mat4>)
        {
            uniform.data = data;
            glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat4>(data)));
        }
        else if constexpr (std::is_same_v<T, glm::mat3x4>)
        {
            uniform.data = data;
            glUniformMatrix3x4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat3x4>(data)));
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            uniform.data = data;
            glUniform1f(uniform.location, std::get<float>(data));
        }
        else if constexpr (std::is_same_v<T, glm::ivec2>)
        {
            uniform.data = data;
            glUniform2iv(uniform.location, 1, glm::value_ptr(std::get<glm::ivec2>(data)));
        }
       else if constexpr (std::is_same_v<T, glm::ivec3>)
        {
            uniform.data = data;
            glUniform3iv(uniform.location, 1, glm::value_ptr(std::get<glm::ivec3>(data)));
        }
        else if constexpr (std::is_same_v<T, int32_t>)
        {
            uniform.data = data;
            glUniform1i(uniform.location, std::get<int32_t>(data));
        }
        else if constexpr(std::is_same_v<T, uint32_t>)
        {
            uniform.data = data;
            glUniform1ui(uniform.location, std::get<uint32_t>(data));
        }

    }, uniform.data, data);
}
