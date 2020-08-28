#include "shader.h"
#include <glad/glad.h>

#include <string>
#include <vector>
#include <log/log.h>
#include "file.h"

namespace 
{
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

        if (view == "vs")
            return GL_VERTEX_SHADER;
        else if (view == "fs")
            return GL_FRAGMENT_SHADER;
        else if (view == "gs")
            return GL_GEOMETRY_SHADER;
        else if (view == "tcs")
            return GL_TESS_CONTROL_SHADER;
        else if (view == "tes")
            return GL_TESS_EVALUATION_SHADER;
        else
            return 0;
    }

    uint32_t load_compile_attach_shader(uint32_t program_id,const std::string &file_name)
    {
        constexpr const int GL_FAILURE = 0;
        std::string filename = file_name;
        GLenum shader_type = shader_type_from_extension(filename);

        if (shader_type == GL_FAILURE) logr::report_error("[graphics] incorrect shader type. filename: {}\n", filename);

        uint32_t shader_id = glCreateShader(shader_type);

        if (shader_id == GL_FAILURE) logr::report_error("[graphics] glCreateShader failed.\n");
        else 
        {
            logr::report("[graphics] glCreateShader succeeded. created shader ID {} \n",shader_id);
        }

        std::string target = {};

        file_to_string(filename, target);
        const char* shader_c_str = target.c_str();
        glShaderSource(shader_id, 1, &shader_c_str, NULL);
        glCompileShader(shader_id);

        GLint max_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);
        std::vector<GLchar> info_log(max_length);        
        glGetShaderInfoLog(shader_id, max_length, &max_length, &info_log[0]);
        
        std::string string_log = std::string(info_log.begin(), info_log.end());
        logr::report("[graphics] shader info log:\n {} \n", string_log);

        GLint shader_compiled = GL_FALSE; 
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);

        if (shader_compiled != GL_TRUE)
        {
            logr::report_warning("[graphics] shader_from_file: unable to compile shader {} \n", filename);
            glDeleteShader(shader_id); // Don't leak the shader.
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
        logr::report("shader info for program {} \n", shader.name);
        bind_shader(shader_manager, shader.name.c_str());

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
            //IC: -1 here is to skip the null character.
            std::string attribute_name(name_data.begin(), name_data.end() - 1);
            shader.attribute_names.push_back(attribute_name);
        }

        //--- Uniforms
        GLint numActiveUniforms = 0;
        glGetProgramInterfaceiv(shader.program_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
        for(size_t unif = 0; unif < numActiveUniforms; ++unif)
        {
            glGetProgramResourceiv(shader.program_id, GL_UNIFORM, unif, properties.size(),
            &properties[0], values.size(), nullptr, &values[0]);
            name_data.resize(values[0]);

            glGetProgramResourceName(shader.program_id, GL_UNIFORM, unif, name_data.size(), NULL, &name_data[0]);
            //IC: -1 here is to skip the null character.
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
                    uniform.data = glm::vec4();
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_FLOAT_VEC4\n", uniform.name);
                    break;
                }
                case GL_FLOAT:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT;
                    uniform.data = float{0.0f};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_FLOAT\n", uniform.name);
                    break;
                }
                case GL_FLOAT_MAT4:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT_MAT4;
                    uniform.data = glm::mat4(1.0f);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_FLOAT_MAT4\n", uniform.name);
                    break;
                }
                case GL_FLOAT_MAT3x4:
                {
                    Uniform uniform;
                    uniform.type = GL_FLOAT_MAT3x4;
                    uniform.data = glm::mat3x4(1.0f);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_FLOAT_MAT3x4\n", uniform.name);
                    break;
                }

                case GL_INT:
                {
                    Uniform uniform;
                    uniform.type = GL_INT;
                    uniform.data = int32_t{0};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_INT\n", uniform.name);
                    break;
                }
                case GL_INT_VEC2:
                {
                    Uniform uniform;
                    uniform.type = GL_INT_VEC2;
                    uniform.data = glm::ivec2(0);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_INT_VEC2\n", uniform.name);
                    break;
                }
                case GL_INT_VEC3:
                {
                    Uniform uniform;
                    uniform.type = GL_INT_VEC3;
                    uniform.data = glm::ivec3(0);
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_INT_VEC3\n", uniform.name);

                    break;
                }
                case GL_SAMPLER_2D:
                {
                    Uniform uniform;
                    uniform.type = GL_SAMPLER_2D;
                    uniform.data = int32_t{0};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_SAMPLER_2D\n", uniform.name);
                    break;
                }
                case GL_UNSIGNED_INT:
                {
                    Uniform uniform;
                    uniform.type = GL_UNSIGNED_INT;
                    uniform.data = uint32_t{0};
                    uniform.name = shader.uniform_names[idx];
                    shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_UNSIGNED_INT\n", uniform.name);
                    break;
                }
                case GL_UNSIGNED_INT_ATOMIC_COUNTER:
                {
                    // Uniform uniform;
                    // uniform.type = GL_UNSIGNED_INT;
                    // uniform.data = uint32_t{0};
                    // uniform.name = shader.uniform_names[idx];
                    // shader.uniforms[uniform.name] = uniform;
                    logr::report("{} : GL_UNSIGNED_INT_ATOMIC_COUNTER\n", shader.uniform_names[idx]);
                    break;
                }

                default:
                {
                    logr::report_error("[Uniform] {}  : UNKNOWN TYPE", shader.uniform_names[idx]);
                    break;
                }
            }
        }

        for (auto &uniform_name: shader.uniform_names)
        {
            shader.uniforms[uniform_name].location = glGetUniformLocation(shader.program_id, uniform_name.c_str());
        }

        glUseProgram(0);

    }


}


//@IC(Sjors):the "none" shader is not any shader.
//@Dependencies: Shader_Manager?
void bind_shader(Shader_Manager& shader_manager, const char* shader_name)
{
    uint32_t shader_id = 0; 
    if (!(shader_name == "none"))
    {
        shader_id = shader_manager.shaders[shader_name].program_id;
    }
    shader_manager.active_shader_name = shader_name;
    glUseProgram(shader_id);
}

uint32_t load_shader(Shader_Manager& shader_manager, const std::string& shader_name)
{
    std::string shader_folder_prefix = "assets/shaders/";
    std::string shader_folder_path = shader_folder_prefix + shader_name;
    uint32_t shader_program = glCreateProgram();
    std::vector<uint32_t> shader_ids;

    if (!std::filesystem::exists(shader_folder_path)) logr::report_error("[graphics] shader folder {} does not exist\n",shader_folder_path);

    for (const auto& file: std::filesystem::directory_iterator(shader_folder_path))
        shader_ids.push_back(load_compile_attach_shader(shader_program, file.path().string()));

    glLinkProgram(shader_program);

    if (!get_shader_link_success(shader_program)) logr::report_error("[graphics] error: shader {} could not be linked\n", shader_folder_path);

    // Detach shaders in any case. we do not need them after they have been attached.
    for (const auto& shader_id: shader_ids)
        glDetachShader(shader_program, shader_id);

    // gather uniforms & attributes.
    //@IC: implicit creation!
    Shader& shader = shader_manager.shaders[shader_name];
    shader.name = shader_name;
    shader.program_id =  shader_program;

    bind_shader(shader_manager, shader_name.c_str());
    get_shader_info(shader_manager, shader);
    
    bind_shader(shader_manager, "none");
    return shader_program;
}