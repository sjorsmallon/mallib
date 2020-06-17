#include "shader_manager.h"


///--- Singleton -------------
Shader_Manager& sm::shader_manager()
{
	static Shader_Manager shader_manager;
	return shader_manager;
}


///--- Get ------
bool graphics::get_shader_link_success(uint32_t program_id)
{
    int gl_params = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &gl_params);
    return gl_params;
}

void graphics::get_shader_info(shader_t& shader)
{

    fmt::print("[graphics] shader info for program {} \n", shader.gl_program_id);

    // bind shader.
    glUseProgram(shader.program_id);

    std::vector<GLchar> name_data(256);
    std::vector<GLenum> properties = {};
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    std::vector<GLint> values(properties.size());

    //--- Attributes
    GLint active_attribute_count = 0;
    glGetProgramInterfaceiv(shader.gl_program_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &active_attribute_count);
    std::vector<std::string> attribute_names;

    for(size_t attribute_idx = 0; attribute_idx < active_attribute_count; ++attribute_idx)
    {
        glGetProgramResourceiv(shader.gl_program_id, GL_PROGRAM_INPUT, attribute_idx, properties.size(),
        &properties[0], values.size(), NULL, &values[0]);
        name_data.resize(values[0]);

        glGetProgramResourceName(shader.gl_program_id, GL_PROGRAM_INPUT, attribute_idx, name_data.size(), NULL, &name_data[0]);

        //IC: -1 here is to skip the null character.
        std::string attribute_name(name_data.begin(), name_data.end() - 1);
        attribute_names.push_back(attribute_name);
    }

    //--- Uniforms
    GLint active_uniform_count = 0;
    glGetProgramInterfaceiv(shader.gl_program_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_uniform_count);
    std::vector<std::string> uniform_names;

    for(size_t uniform_idx = 0; uniform_idx < active_uniform_count; ++uniform_idx)
    {
        glGetProgramResourceiv(shader.gl_program_id, GL_UNIFORM, uniform_idx, properties.size(),
        &properties[0], values.size(), NULL, &values[0]);
        name_data.resize(values[0]); //The length of the name.

        glGetProgramResourceName(shader.gl_program_id, GL_UNIFORM, uniform_idx, name_data.size(), NULL, &name_data[0]);

        //IC: -1 here is to skip the null character.
        std::string uniform_name(name_data.begin(), name_data.end() - 1);//
        uniform_names.push_back(uniform_name);
    }

    //--- Uniform types
    std::vector<uint32_t> uniform_indices(uniform_names.size());
    std::vector<const char*> string_pointers;
    std::vector<int32_t> uniform_types(uniform_names.size());

    for (auto &uniform_name: shader.uniform_names)
    {
        string_pointers.push_back(uniform_name.c_str());
    }
    assert(shader.uniform_names.size() == string_pointers.size());
    assert(uniform_indices.size() == shader.uniform_names.size());

    glGetUniformIndices(
        shader.gl_program_id,
        uniform_names.size(),
        string_pointers.data(),
        uniform_indices.data()
        );

    glGetActiveUniformsiv(
        shader.gl_program_id,
        uniform_names.size(),
        uniform_indices.data(),
        GL_UNIFORM_TYPE, 
        uniform_types.data()
        );

    for (size_t idx = 0; idx != shader.uniform_names.size(); ++idx)
    {
        auto uniform_type = uniform_types[idx];
        switch(uniform_type)
        {
            case GL_FLOAT_VEC3:
            {
                uniform_t uniform;
                uniform.name = uniform_names[idx];

                uniform.data = mgl::vec3{0};
                uniform.gl_type = GL_FLOAT_VEC3;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());

                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_VEC3\n", uniform.name);

                break;
            }

            case GL_FLOAT_VEC4:
            {
                
                uniform_t uniform;
                uniform.name = uniform_names[idx];

                uniform.data = mgl::vec4{0};
                uniform.gl_type = GL_FLOAT_VEC4;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());

                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_VEC4\n", uniform.name);

                break;
            }
            case GL_FLOAT:
            {
                uniform_t uniform;
                uniform.name = uniform_names[idx];

                uniform.data = float{0.0f};
                uniform.gl_type = GL_FLOAT;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());


                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT\n", uniform.name);

                break;
            }
            case GL_FLOAT_MAT4:
            {
                uniform_t uniform;
                uniform.name = uniform_names[idx];

                uniform.data = mgl::mat4_identity();
                uniform.gl_type = GL_FLOAT_MAT4;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());

                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_MAT4\n", uniform.name);

                break;
            }
            case GL_FLOAT_MAT3:
            {
                uniform_t uniform;
                uniform.name = uniform_names[idx];

                uniform.data = mgl::mat3_identity();
                uniform.gl_type = GL_FLOAT_MAT3;
                uniform.gl_index = uniform_indices[idx];
                uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());


                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_MAT3\n", uniform.name);

                break;
            }
            case GL_INT:
            {
                uniform_t uniform;
                uniform.name = uniform_names[idx];

                uniform.data = int32_t{0};
                uniform.gl_type = GL_INT;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());


                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_INT\n", uniform.name);

                break;
            }
            case GL_INT_VEC2:
            {
                uniform_t uniform;
                uniform.name = shader.uniform_names[idx];

                uniform.data = mgl::ivec2{0};
                uniform.gl_type = GL_INT_VEC2;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());

                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_INT_VEC2\n", uniform.name);

                break;
            }
            case GL_UNSIGNED_INT:
            {
                uniform_t uniform;
                uniform.name =     uniform_names[idx];

                uniform.data = uint32_t{0};
                uniform.gl_type = GL_UNSIGNED_INT;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());

                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_UNSIGNED INT\n", uniform.name);

                break;
            }
            case GL_SAMPLER_2D:
            {
                uniform_t uniform;
                uniform.name = uniform_names[idx];

                uniform.data = int32_t{0};
                uniform.gl_type = GL_SAMPLER_2D;
                uniform.gl_index = uniform_indices[idx];
				uniform.gl_location = glGetUniformLocation(shader.gl_program_id, uniform.name.c_str());

                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_SAMPLER_2D\n", uniform.name);

                break;
            }

            default:
            {
                fmt::print(" {},: UNKNOWN TYPE!\n", shader.uniform_names[idx]);

                break;
            }
        }
    }

    // unbind shader.
    glUseProgram(0);
}


///--- Set -----
sm::set_shader(Shader_Manager& manager, const std::string& shader_name)
{
	assert(manager.shaders.find(shader_name) != manager.shaders.end());

    uint32_t shader_id = manager.shaders[shader_name];
    
    glUseProgram(shader_id);
    manager.active_shader_name = shader_name;
	manager.active_shader_id   = shader_id; 
}

///---- regular

// shader_name refers to the relative folder path. e.g. "normals" for the folder "assets/shaders/normals".
void sm::load_shader(const Shader_Manager& manager, const std::string& shader_name)
{
    std::string shader_folder_path = "assets/shaders/" + shader_name;
    uint32_t shader_program = glCreateProgram();

    std::vector<uint32_t> shader_ids; // one for each shader type.

    if (!fs::exists(shader_folder_path))
        fmt::print("[graphics] error: shader folder {} does not exist.", shader_folder_path);

    for (const auto& file: fs::directory_iterator(shader_folder_path))
    {    	
        shader_ids.push_back(
        	sm::load_compile_attach_shader(
        		shader_program,
        		 file.path().string()
        	)
        );
    }

    glLinkProgram(shader_program);

    if (!sm::get_gl_link_success(manager, shader_program))
        fmt::print("[graphics] error: shader {} could not be linked.", shader_folder_path);

    // detach after linking, no need to keep them around.
    for (const auto& shader_id: shader_ids)
        glDetachShader(shader_program, shader_id);

    // gather uniforms & attributes.
    shader_t& shader = manager.shaders[shader_name];
    shader.gl_program_id = shader_program;
    sm::get_gl_shader_info(manager, shader);
 
    fmt::print("[graphics] uniforms: \n");
    for (auto&& uniform_name: shader.uniform_names)
        fmt::print("    {} \n", uniform_name);

    fmt::print("[graphics] attributes: \n");
    for (auto&& attribute_name: shader.attribute_names)
        fmt::print("    {} \n", attribute_name);

}



//--- Pre:  active shader.
void sm::update_uniform(const std::string& uniform_name, uniform_type data)
{
	sm::Shader_Manager& manager = sm::shader_manager();
    auto& uniforms =  manager.shaders[manager.active_shader_name].uniforms;

    if (uniforms.find(uniform_name) == uniforms.end())
    {
        fmt::print("key {} does not exist.\n", uniform_name);
    }

    Uniform& uniform = uniforms[uniform_name];
    std::visit([&](auto&& uniform_data, auto&& new_data)
    {
        using T = std::decay_t<decltype(uniform_data)>;
        using my_T = std::decay_t<decltype(new_data)>;
        if constexpr(!(std::is_same_v<T, my_T>))
        {
            fmt::print("[graphics] update_uniform: type mismatch. uniform name: {}\n",uniform.name);
        }
        else if constexpr (std::is_same_v<T, mgl::vec4>)
        {
            uniform.data = data;
            glUniform4fv(uniform.location, 1, mgl::value_ptr(std::get<mgl::vec4>(uniform.data)));
        }
        else if constexpr (std::is_same_v<T, mgl::vec3>)
        {
            uniform.data = data;
            glUniform3fv(uniform.location, 1,  mgl::value_ptr(std::get<mgl::vec3>( uniform.data)));
        }

        else if constexpr (std::is_same_v<T, mgl::mat3>)
        {
            uniform.data = data;
            glUniformMatrix3fv(uniform.location, 1, GL_TRUE, mgl::value_ptr(std::get<mgl::mat3>(uniform.data)));
        }
        else if constexpr (std::is_same_v<T, mgl::mat4>)
        {
            uniform.data = data;
            glUniformMatrix4fv(uniform.location, 1, GL_FALSE, mgl::value_ptr( std::get<mgl::mat4>( uniform.data)));
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            uniform.data = data;
            glUniform1f(uniform.location, std::get<float>(data));
        }
        else if constexpr (std::is_same_v<T, mgl::ivec2>)
        {
            uniform.data = data;
            glUniform2iv(uniform.location, 1,  mgl::value_ptr(std::get<mgl::ivec2>(uniform.data)));
        }
        else if constexpr (std::is_same_v<T, int32_t>)
        {
            uniform.data = data;
            glUniform1i(uniform.location, std::get<int32_t>(uniform.data));
        }
        else if constexpr(std::is_same_v<T, uint32_t>)
        {
            uniform.data = data;
            glUniform1ui(uniform.location, std::get<uint32_t>(uniform.data));
        }

    }, uniform.data, data);
}








//--- helper / local functions.

uint32_t sm::load_compile_attach_shader(uint32_t program, std::string file_name)
{
    constexpr const int GL_FAILURE = 0;
    GLenum shader_type = graphics::shader_type_from_extension(filename);

    if (shader_type == GL_FAILURE)
        fmt::print("[graphics] incorrect shader extension.\n");
    uint32_t shader_id = glCreateShader(shader_type);

    if (shader_id == GL_FAILURE)
        fmt::print("[graphics] glCreateShader failed.\n");
    else
        fmt::print("[graphics] glCreateShader succeeded. created shader ID {}\n", shader_id);

    std::string target = {};

    file::file_to_string(filename, target);

    const char* shader_c_str = target.c_str();
    glShaderSource(shader_id, 1, &shader_c_str, NULL);
    glCompileShader(shader_id);

    GLint shader_compiled = GL_FALSE; 
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);
    if (shader_compiled != GL_TRUE)
    {
        fmt::print("[graphics] shader_from_file: unable to compile shader {}\n", filename);

        GLint max_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);
        std::vector<GLchar> error_log(max_length);        
        glGetShaderInfoLog(shader_id, max_length, &max_length, &error_log[0]);
        
        std::string string_log = std::string(error_log.begin(), error_log.end());
        fmt::print("[graphics] shader error log:\n {}\n ", string_log);
        glDeleteShader(shader_id); // Don't leak the shader.

    }
    else
    {
        fmt::print("[graphics] shader_from_file: successfully compiled {}\n", filename);
        glAttachShader(program, shader_id);
    }

    return shader_id;
}

uint32_t graphics::shader_type_from_extension(const std::string& filename)
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
    // else if (view == "compute")
    //     return GL_COMPUTE_SHADER;
    else
        return 0;
}