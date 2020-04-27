#include <graphics.h>

#undef max
#undef min
#include <fmt/core.h>
#include <filesystem> // for directory iteration for the shader.
#include <stdint.h>
#include <string>
#include <sstream>
#include <Wingdi.h>
#include <stdlib.h>

//--- OpenGL
#include <gl_lite/gl_lite.h>

//--- Imgui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_opengl3.h>

//--- file includes to sort out.
#include <on_leaving_scope.h>
#include <globals.h>
#include <file.h>
#include <scene.h>
#include <game.h> // game time?
#include <camera.h>

#include <mgl/mat.h>
#include <mgl/mat4.h>
#include <mgl/mat3.h>

#include <glm/gtx/string_cast.hpp> // to_string
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>   // rotation_matrix 

#define BUFFER_OFFSET(i) ((void*)(i)) //hacky macro for offset.

namespace fs = std::filesystem;

cam::Camera& graphics::active_camera()
{
    static cam::Camera active_camera;
    return active_camera;
}

std::map<std::string, graphics::Buffers>& graphics::buffers()
{
    static std::map<std::string, graphics::Buffers> buffers;
    return buffers;
}

void graphics::init_graphics()
{ 
    graphics::global_Win32_context().device_context = globals.device_context;
    graphics::global_Win32_context().window_handle = globals.window_handle;

    graphics::active_camera() = cam::default_camera();
    graphics::init_opengl();
    graphics::init_imgui();
    graphics::clear_buffer_bits();

}

// static
void graphics::init_opengl()
{
    //@NOte::init gl_lite only after the gl_context has been created
    // (which is done in the win32 section of the program, since that is OS related.)
    gl_lite_init();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(40.0f/255.0f, 45.0f/255.0f, 54.0f/ 255.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//static
void graphics::init_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(graphics::global_Win32_context().window_handle);
    std::string glsl_version = "#version 430";
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

void graphics::init_texture_settings(std::map<std::string, asset::Texture>& textures)
{
    for (auto& [texture_name, texture]: textures)
    {
        glGenTextures(1, &texture.gl_texture_id);

        uint32_t free_texture_frame = graphics::next_free_active_texture_ID();
        glActiveTexture(GL_TEXTURE0 + free_texture_frame);
        glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
       
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB8,
            texture.dimensions.x,
            texture.dimensions.y,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            texture.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    //@TODO: generate mipmaps.
}

void graphics::update_active_camera(io::Mouse_State &mouse_state)
{
    static float camera_z_accumulator = 2.0f;
    static float x_rotation_accumulator = 0.0f;
    static float z_rotation_accumulator = 0.0f;

    using namespace cam;

    auto& camera = graphics::active_camera();
    switch (camera.control_mode)
    {
        case Control_Mode::CAM_ORBIT:
        {
            if (mouse_state.lmb_pressed)
            {
                x_rotation_accumulator += mouse_state.pos_delta_x * 0.5f;
                x_rotation_accumulator =  fmod(x_rotation_accumulator, 360.0f);
                z_rotation_accumulator += mouse_state.pos_delta_y * 0.5f;
                z_rotation_accumulator =  fmod(z_rotation_accumulator, 360.0f);
            }
   
            if (mouse_state.scroll_delta_y != 0.0f)
            {
                camera_z_accumulator = (mouse_state.scroll_delta_y > 0.0f) ? camera_z_accumulator - 0.05f : camera_z_accumulator + 0.05f;                 
            }

            mgl::vec3 camera_position{0.0f,0.0f, camera_z_accumulator};
            mgl::vec3 target_position{0.0f, 0.0f, -1.0f}; 
            mgl::vec3 up_vector{0.0f, 1.0f, 0.0f};        
            mgl::mat4 rotation_matrix = mgl::rotate(mgl::mat4_identity(), x_rotation_accumulator, 0, z_rotation_accumulator);

            glm::mat4 actual_rotation_matrix = glm::rotate(static_cast<float>(x_rotation_accumulator), glm::vec3(0.0f, 0.0f, 1.0f)) *  // z
                                               glm::rotate(static_cast<float>(z_rotation_accumulator), glm::vec3(1.0f, 0.0f, 0.0f)); // x

            fmt::print("my rotation matrix: {}\n", rotation_matrix);
            fmt::print("actual rotation matrix: {}\n", glm::to_string(actual_rotation_matrix));
                
            up_vector       = mgl::make_vec3(rotation_matrix * mgl::make_vec4(up_vector, 1.0f));
             camera_position = mgl::make_vec3(rotation_matrix * mgl::make_vec4(camera_position, 1.0f));

            mgl::mat4 view_matrix = mgl::look_at(camera_position, target_position, up_vector);

            // @Note: near_z and far_z should be positive in this context (DISTANCE to camera).
            const float fov_in_degrees     = 45.0f;
            const float perspective_near_z = 0.1f;
            const float perspective_far_z  = 200.0f;
            const float aspect_ratio = static_cast<float>(globals.window_width) / static_cast<float>(globals.window_height);
            mgl::mat4 projection_matrix = mgl::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);


            // glm::mat4  actual_projection_matrix = glm::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);

            update_uniform("projection_matrix", projection_matrix);
            update_uniform("view_matrix", view_matrix);

            break;
        }
        case Control_Mode::CAM_ISOMETRIC:
        {
            break;
        }
        case Control_Mode::CAM_FPS:
        {

            
            break;
        }

        default:
        {
            break;
        }
    }
}


/// returns a free active texture ID and increments the value.
uint32_t graphics::next_free_active_texture_ID()
{
    static uint32_t free_texture_frame;
    uint32_t new_free_texture_frame = free_texture_frame;
    free_texture_frame += 1;

    return new_free_texture_frame;
}


//------ render

void graphics::render_game_3d()
{

    // uint32_t active_shader_id = graphics::active_shader_id();
    // auto defer_shader_state = On_Leaving_Scope([]{graphics::set_shader("gouraud");});

    // // render_3d_left_handed_perspective(active_shader_id); // perspective matrix.

    // if (active_shader_id == graphics::shaders()["gouraud"])
    // {
    //     update_uniform("light_position", mgl::vec3{50.0f, 100.0f, 100.0f});
    //     update_uniform("light_color", mgl::vec3{0.0f, 0.0f, 0.1f});
    //     update_uniform("material", mgl::vec4{0.9f, 0.9f, 0.9f, 32.0f});
    // }

    // if (active_shader_id == graphics::shaders()["cel"])
    // {
    //     update_uniform("light_position", mgl::vec3{50.0f, 100.0f, 100.0f});
    //     update_uniform("light_color", mgl::vec3{0.0f, 0.0f, 0.1f});
    //     update_uniform("material", mgl::vec4{0.9f, 0.9f, 0.9f, 32.0f});
    // }


    // glBindVertexArray(graphics::buffers()["cat.obj"].VAO);

    // mgl::mat4 view_matrix = std::get<mgl::mat4>(graphics::shader_info()[graphics::active_shader_name()].uniforms["view_matrix"].data);

    // for (auto &set_piece: graphics::active_scene().set_pieces)
    // {
    //     mgl::mat4 model_matrix = mgl::model_from_xform_state(set_piece.xform_state);
    //     mgl::mat4 model_view_matrix =  model_matrix * view_matrix; // reverse order. 
    //     mgl::mat3 model_normal_matrix = mgl::normal_transform(model_matrix);

    //     update_uniform("model_matrix", model_matrix);
    //     update_uniform("model_normal_matrix", model_normal_matrix);

    //     if (active_shader_id == graphics::shaders()["gouraud"])
    //     {
    //         asset::Texture& texture = asset::texture_data()[set_piece.texture_name];
    //         glActiveTexture(GL_TEXTURE0 + texture.gl_texture_frame);
    //         glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
    //         update_uniform("texture_uniform",  texture.gl_texture_frame);
    //     }

    //     //@FIXME: for now, we invoke draw after every object. 
    //     const auto& object_data = asset::obj_data()[set_piece.model_name];
    //     glDrawArrays(GL_TRIANGLES,0, object_data.vertices.size());
    // }


    render_centroid_axes();


    glBindVertexArray(0);
}

// render axes for the 0,0,0 point.
void graphics::render_centroid_axes()
{

    mgl::vec4 x_axis_end{1.0f,0.0f,0.0f, 1.0f};
    mgl::vec4 y_axis_end{0.0f,1.0f,0.0f, 1.0f};
    mgl::vec4 z_axis_end{0.0f,0.0f,1.0f, 1.0f};
    mgl::vec4 zero_start{0.0f,0.0f,0.0f,1.0f};

    graphics::set_shader("cel");
    mgl::mat4 view_matrix       = std::get<mgl::mat4>(graphics::shader_info()["cel"].uniforms["view_matrix"].data);
    mgl::mat4 projection_matrix = std::get<mgl::mat4>(graphics::shader_info()["cel"].uniforms["projection_matrix"].data);

    graphics::set_shader("line_position_color");

    x_axis_end =  projection_matrix * view_matrix * x_axis_end;
    y_axis_end =  projection_matrix * view_matrix * y_axis_end;
    z_axis_end =  projection_matrix * view_matrix * z_axis_end;
    zero_start =  projection_matrix * view_matrix * zero_start;

    mgl::vec4 x_axis_color{1.0f, 0.0f, 0.0f, 1.0f};
    mgl::vec4 y_axis_color{0.0f, 1.0f, 0.0f, 1.0f};
    mgl::vec4 z_axis_color{0.0f, 0.0f, 1.0f, 1.0f};

    glBindVertexArray(graphics::buffers()["line_position_color"].VAO);

    std::vector<mgl::vec4> data{
        zero_start,
        x_axis_color,
        x_axis_end,
        x_axis_color,
        zero_start,
        y_axis_color,
        y_axis_end,
        y_axis_color,
        zero_start,
        z_axis_color,
        z_axis_end,
        z_axis_color
    };
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<int32_t>(data.size() * sizeof(mgl::vec4)),
        data.data(),
        GL_STATIC_DRAW);

    glDrawArrays(GL_LINES,0, data.size() / 2);



}


void graphics::render_ui()
{
      // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    bool show_demo_window = false;
    if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

    std::vector<const char *> shader_names;
    for (auto& [shader_name, _]: graphics::shader_info())
    {
        shader_names.push_back(shader_name.c_str());
    }
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        ImGui::Begin("Menu");                        
        ImGui::Text("Render Settings");

        // List box
        static int selected_shader = 1;
        ImGui::ListBox("Shader Mode", &selected_shader, shader_names.data(), shader_names.size(), 4);
        graphics::set_shader(shader_names[selected_shader]);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    
    ImGui::Render();

    glViewport(0, 0, globals.window_width, globals.window_height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// static
void graphics::clear_buffer_bits()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void graphics::render_frame()
{
    graphics::render_game_3d();
}

void graphics::swap_buffers()
{
    SwapBuffers(graphics::global_Win32_context().device_context);
}

scene::Scene& graphics::active_scene()
{
    static scene::Scene active_scene;
    return active_scene;
}

graphics::Win32_Context& graphics::global_Win32_context() //@cleanup: i don't like this.
{
    static Win32_Context context;
    return context;
}













///--- Shaders --------------------------------------------------
std::map<std::string, uint32_t>& graphics::shaders()
{
    static std::map<std::string, uint32_t> shaders;
    return shaders;
}

std::map<std::string, graphics::Shader>& graphics::shader_info()
{
    static std::map<std::string, graphics::Shader> shader_info;
    return shader_info;
}


// shader_name refers to the relative folder path. e.g. "normals" for the folder "assets/shaders/normals".
uint32_t graphics::load_shader(const std::string& shader_name)
{
    std::string shader_folder_path = "assets/shaders/" + shader_name;
    uint32_t shader_program = glCreateProgram();
    std::vector<uint32_t> shader_ids(8);

    if (!fs::exists(shader_folder_path))
        fmt::print("[graphics] error: shader folder {} does not exist.", shader_folder_path);

    for (const auto& file: fs::directory_iterator(shader_folder_path))
        shader_ids.push_back(graphics::load_compile_attach_shader(shader_program, file.path().string()));

    glLinkProgram(shader_program);

    if (!graphics::shader_link_succeeded(shader_program))
        fmt::print("[graphics] error: shader {} could not be linked.", shader_folder_path);

    for (const auto& shader_id: shader_ids)
        glDetachShader(shader_program, shader_id);

    // gather uniforms & attributes.
    graphics::Shader& shader = graphics::shader_info()[shader_name];
    shader.program_id =  shader_program;
    get_shader_info(shader);

    fmt::print("[graphics] uniforms: \n");
    for (auto&& uniform_name: shader.uniform_names)
        fmt::print("    {} \n", uniform_name);

    fmt::print("[graphics] attributes: \n");
    for (auto&& attribute_name: shader.attribute_names)
        fmt::print("    {} \n", attribute_name);

    return shader_program;
}

std::string& graphics::active_shader_name()
{
    static std::string active_shader_name;
    return active_shader_name;
}

uint32_t& graphics::active_shader_id()
{
    static uint32_t active_shader_id;
    return active_shader_id;
}

void graphics::set_shader(const std::string& shader_name)
{
    uint32_t shader_id = graphics::shaders()[shader_name];
    glUseProgram(shader_id);
    graphics::active_shader_name() = shader_name;
    graphics::active_shader_id() = shader_id; 
}

uint32_t graphics::load_compile_attach_shader(uint32_t program, std::string file_name)
{
    constexpr const int GL_FAILURE = 0;
    std::string filename = file_name;
    GLenum shader_type = graphics::shader_type_from_extension(filename);

    if (shader_type == GL_FAILURE)
        fmt::print("[graphics] incorrect shader type.\n");
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


void graphics::get_shader_info(graphics::Shader& shader)
{

    fmt::print("[graphics] shader info for program {} \n", shader.program_id);
    glUseProgram(shader.program_id);

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
        &properties[0], values.size(), NULL, &values[0]);
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
        &properties[0], values.size(), NULL, &values[0]);
        name_data.resize(values[0]); //The length of the name.
        glGetProgramResourceName(shader.program_id, GL_UNIFORM, unif, name_data.size(), NULL, &name_data[0]);
        //IC: -1 here is to skip the null character.
        std::string uniform_name(name_data.begin(), name_data.end() - 1);//
        shader.uniform_names.push_back(uniform_name);
    }

    //--- Uniform types
    std::vector<uint32_t> uniform_indices(shader.uniform_names.size());
    std::vector<const char*> string_pointers;
    std::vector<int32_t> uniform_types(shader.uniform_names.size());

    for (auto &uniform_name: shader.uniform_names)
    {
        string_pointers.push_back(uniform_name.c_str());
    }
    assert(shader.uniform_names.size() == string_pointers.size());
    assert(uniform_indices.size() == shader.uniform_names.size());

    glGetUniformIndices(
        shader.program_id,
        shader.uniform_names.size(),
        string_pointers.data(),
        uniform_indices.data()
        );

    glGetActiveUniformsiv(
        shader.program_id,
        shader.uniform_names.size(),
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
                Uniform uniform;
                uniform.type = GL_FLOAT_VEC3;
                uniform.data = mgl::vec3{0};
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_VEC3\n", uniform.name);
                break;
            }

            case GL_FLOAT_VEC4:
            {
                
                Uniform uniform;
                uniform.type = GL_FLOAT_VEC4;
                uniform.data = mgl::vec4{0};
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_VEC4\n", uniform.name);
                break;
            }
            case GL_FLOAT:
            {
                Uniform uniform;
                uniform.type = GL_FLOAT;
                uniform.data = float{0.0f};
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT\n", uniform.name);

                break;
            }
            case GL_FLOAT_MAT4:
            {
                Uniform uniform;
                uniform.type = GL_FLOAT_MAT4;
                uniform.data = mgl::mat4_identity();
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_MAT4\n", uniform.name);
                break;
            }
            case GL_FLOAT_MAT3:
            {
                Uniform uniform;
                uniform.type = GL_FLOAT_MAT3;
                uniform.data = mgl::mat3_identity();
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_FLOAT_MAT3\n", uniform.name);
                break;
            }
            case GL_INT:
            {
                Uniform uniform;
                uniform.type = GL_INT;
                uniform.data = int32_t{0};
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_INT\n", uniform.name);
                break;
            }
            case GL_INT_VEC2:
            {
                Uniform uniform;
                uniform.type = GL_INT_VEC2;
                uniform.data = mgl::ivec2{0};
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_INT_VEC2\n", uniform.name);
                break;
            }
            case GL_UNSIGNED_INT:
            {
                Uniform uniform;
                uniform.type = GL_UNSIGNED_INT;
                uniform.data = uint32_t{0};
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_UNSIGNED INT\n", uniform.name);

                break;
            }
            case GL_SAMPLER_2D:
            {
                Uniform uniform;
                uniform.type = GL_SAMPLER_2D;
                uniform.data = int32_t{0};
                uniform.name = shader.uniform_names[idx];
                shader.uniforms[uniform.name] = uniform;
                fmt::print(" {}, GL_SAMPLER_2D\n", uniform.name);
            }

            default:
            {
                fmt::print(" {},: UNKNOWN TYPE!\n", shader.uniform_names[idx]);
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

bool graphics::shader_link_succeeded(uint32_t program_id)
{
    int gl_params = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &gl_params);
    return gl_params;
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

//--- Pre:  active shader.
void graphics::update_uniform(const std::string& uniform_name, uniform_t data)
{
    auto& uniforms = graphics::shader_info()[graphics::active_shader_name()].uniforms;
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
            glUniformMatrix4fv(uniform.location, 1, GL_TRUE, mgl::value_ptr( std::get<mgl::mat4>( uniform.data)));
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



///--- Render Modes -------------------------------------------------------

void graphics::render_2d_left_handed_dc(const uint32_t active_shader_id)
{
    //@Note: do NOT forget this.
    const bool row_major = true;

    const uint32_t window_height = globals.window_height;
    const uint32_t window_width = globals.window_width;
    const float top   = window_height; // viewport 
    const float bot   = 0.0f;
    const float left  = 0.0f;
    const float right = window_width; // viewport
    const float z_near  = 0.0f;
    const float z_far   = 1.0f; // near and far are reserved by windows???


    mgl::mat4 projection_matrix = mgl::ortho(left, right, top, bot, z_near, z_far); 
    glUniformMatrix4fv(glGetUniformLocation(graphics::shaders()["text"], "projection_matrix"), 1, row_major, &projection_matrix[0][0]);
}

void graphics::render_3d_left_handed_perspective(const uint32_t active_shader_id)
{
    //@Note: do NOT forget this.
    const bool row_major = true;

    //@Note: view matrix is identity (i.e. opengl_left_handed, mgl::vec3(0.0,0.0,0.)
    mgl::vec3 camera_position{0.0f,0.0f, 2.0f};
    mgl::vec3 target_position{0.0f, 0.0f, 0.0f};
    mgl::vec3 up_vector{0.0f, 1.0f, 0.0f};
    mgl::mat4 view_matrix = mgl::look_at(camera_position, target_position, up_vector);
   
    // @Note: near_z and far_z should be positive in this context (DISTANCE to camera).
    const float fov_in_degrees     = 45.0f;
    const float perspective_near_z = 0.1f;
    const float perspective_far_z  = 100.0f;
    const float aspect_ratio = static_cast<float>(globals.window_width) / static_cast<float>(globals.window_height);
    mgl::mat4 projection_matrix = mgl::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);

    update_uniform("projection_matrix", projection_matrix);
    update_uniform("view_matrix", view_matrix);
}



// Jon Blow's render_frame call.
// void graphics::render_frame()
// {
    
//     graphics::set_render_target(0, hdr_buffer);
//     graphics::set_depth_target(0, hdr_depth);

//     glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     graphics::draw_game_3d();

//     graphics::set_render_target(0, back_buffer);
//     graphics::set_depth_target(0, nullptr); // clear_depth_target;

//     graphics::resolve_to_ldr();

//     glEnable(GL_BLEND);
//     Hud_Properties hud = {};
//     hud.start_x = 100; 
//     hud.start_y = 100;
//     hud.width   = 500;
//     hud.height  = 500; 

//     graphics::draw_hud(hud);

//     graphics::swap_buffers();

// }