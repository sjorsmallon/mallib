#include "graphics.h"

#undef max
#undef min
#include <fmt/core.h>
#include <filesystem> // for directory iteration for the shader.
#include <stdint.h>
#include <string>
#include <sstream>
#include <Wingdi.h>
#include <stdlib.h>

#include "../on_leaving_scope/on_leaving_scope.h"
#include "../win32/gl_lite.h"
#include "../win32/globals.h"
#include "../file/file.h"
#include "../mat/mat.h"
#include "../mat4/mat4.h"
#include "../mat3/mat3.h" // for normal matrix.
#include "../scene/scene.h"
#include "../game/game.h" // game time?

#define BUFFER_OFFSET(i) ((void*)(i)) //hacky macro for offset.
namespace fs = std::filesystem;

//@TODO: set up texture bookkeeping. Who gets which GL_TEXTURE0 + N?
//@TODO: set up shader initialization. Do we want a shared shader? do we want to load shaders per folder?
std::map<std::string, graphics::Buffers>& graphics::buffers()
{
    static std::map<std::string, graphics::Buffers> buffers;
    return buffers;
}

void graphics::init_graphics()
{ 
    graphics::global_Win32_context().device_context = globals.device_context;
    graphics::init_opengl();
    graphics::clear_buffers();
}

void graphics::init_opengl()
{
    //@NOte::init gl_lite only after the gl_context has been created
    // (which is done in the win32 section of the program, since that is OS related.)
    gl_lite_init();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.2f, 0.8f, 0.3f, 1.0f);
    //@Refactor: These OpenGL settings are used for font. should we move it there?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

uint32_t graphics::load_shader(const std::string& shader_folder_path)
{
    uint32_t shader_program = glCreateProgram();
    std::vector<uint32_t> shader_ids;

    if (!fs::exists(shader_folder_path))
        fmt::print("[graphics] error: shader folder {} does not exist.", shader_folder_path);

    for (const auto& file: fs::directory_iterator(shader_folder_path))
        shader_ids.push_back(graphics::load_compile_attach_shader(shader_program, file.path().string()));

    glLinkProgram(shader_program);
    if (!graphics::link_success(shader_program))
        fmt::print("[graphics] error: shader {} could not be linked.", shader_folder_path);
    for (const auto& shader_id: shader_ids)
        glDetachShader(shader_program, shader_id);

    return shader_program;
}

/// returns a free active texture ID and increments the value.
uint32_t graphics::next_active_texture_id()
{
    static uint32_t active_texture_id;
    uint32_t free_active_texture_id = active_texture_id;
    active_texture_id += 1;

    return free_active_texture_id;
}

void graphics::set_shader(const std::string& shader_name)
{
    glUseProgram(graphics::shaders()[shader_name]);
}

void graphics::init_texture_settings(std::map<std::string, asset::Texture>& textures)
{

    for (auto& [texture_name, texture]: textures)
    {
        glGenTextures(1, &texture.gl_texture_id);

        glActiveTexture(GL_TEXTURE0);
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
}

// draw_game-3d:
// - set active shader
// - check whether or not view / projection matrix have changed before updating
// - for each light in the scene:
//       -- do something.
// - for all types of object
// - bind the VAO
// - for all instances of that type (now implemented as - for each object in the active scene:)
//       - maybe render texture
//       - draw object 

void graphics::draw_game_3d()
{
    uint32_t active_shader_id = 0;
    bool render_isophotes = true;
    if (render_isophotes)
    {
        graphics::set_shader("isophotes");
        active_shader_id = graphics::shaders()["isophotes"];
    }
    auto defer_shader_state = On_Leaving_Scope([]{graphics::set_shader("gouraud");});
    render_3d_left_handed_perspective(active_shader);

    if (active_shader_id == graphics::shaders()["gouraud"])
    {
        int32_t light_position_location = glGetUniformLocation(active_shader, "light_position");
        int32_t light_color_location    = glGetUniformLocation(active_shader, "light_color");
        int32_t material_location       = glGetUniformLocation(active_shader, "material");
        Vec3 light_position = {0.0f, 0.0f, -0.5f};
        Vec3 light_color =    {1.0f, 1.0f, 1.0f};
        Vec4 material =       {0.4f, 0.6f, 0.8f, 64.0f};
        glUniform3fv(light_position_location, 1, &light_position.data[0]);
        glUniform3fv(light_color_location,    1, &light_color.data[0]);
        glUniform4fv(material_location,       1, &material.data[0]);
    }

    glBindVertexArray(graphics::buffers()["cat.obj"].VAO);
    const int32_t model_matrix_location = glGetUniformLocation(active_shader, "model_matrix");
    const int32_t normal_transform_matrix_location = glGetUniformLocation(active_shader_id, "normal_transform");

    bool row_major = true;
    for (auto &set_piece: graphics::active_scene().set_pieces)
    {
        //@Refactor: should all xform_state quaternions be unit quaternions?
        set_piece.xform_state.scale = 2.0;

        Mat4 model_matrix = mat::model_from_xform_state(set_piece.xform_state);
        glUniformMatrix4fv(model_matrix_location, 1, row_major, &model_matrix[0][0]);

        Mat3 normal_transform_matrix = mat::normal_transform(model_matrix);
        glUniformMatrix3fv(normal_transform_matrix_location, 1, row_major, &normal_transform_matrix[0][0]);

        if (active_shader_id == graphics::shaders()["gouraud"])
        {
            //@NOTE!!! this uniform does not take the texture ID, but takes the N from GL_TEXTURE0 +N!!!
            // so the question is: which gl_texture owns which textures?
            // An analogy we can use is that texture_2D is a picture frame to which we bind the "actual" picture using the texture_id.
            // gl_texture0 is the wall on which the picture frames hang. Sounds good to me.
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, asset::texture_data()[set_piece.texture_name].gl_texture_id);
            const int32_t texture_location = glGetUniformLocation(active_shader_id, "texture_uniform");
            glUniform1i(texture_location, 1);
        }

        //@FIXME: for now, we invoke draw after every object. 
        const auto& object_data = asset::obj_data()[set_piece.model_name];
        glDrawArrays(GL_TRIANGLES,0, object_data.vertices.size());
        // glUniform1i(d_textureLocation, 0);
    }
}

// static
void graphics::clear_buffers()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics::render_frame()
{
    graphics::draw_game_3d();
}

void graphics::swap_buffers()
{
    SwapBuffers(graphics::global_Win32_context().device_context);
}

////
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
        fmt::print("[graphics] shader error log: {}\n", string_log);

        glDeleteShader(shader_id); // Don't leak the shader.
    }
    else
    {
        fmt::print("[graphics] shader_from_file: successfully compiled {}\n", filename);
        glAttachShader(program, shader_id);
    }

    return shader_id;
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


// helper functions
//@TODO:turn this into multiple things? i.e. get_shader_info,.
// get_program_info, get_opengl_state.
// static
void graphics::get_shader_info(uint32_t prog)
{
    fmt::print("shader info for program {}:\n", prog);

    std::vector<GLchar> name_data(256);
    std::vector<GLenum> properties = {};
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    std::vector<GLint> values(properties.size());

    // PROGRAM_ATTRIBUTES
    GLint numActiveAttribs = 0;
    glGetProgramInterfaceiv(prog, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);

    for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        glGetProgramResourceiv(prog, GL_PROGRAM_INPUT, attrib, properties.size(),
        &properties[0], values.size(), NULL, &values[0]);

        name_data.resize(values[0]);
        glGetProgramResourceName(prog, GL_PROGRAM_INPUT, attrib, name_data.size(), NULL, &name_data[0]);
        std::string name(name_data.begin(), name_data.end());
        fmt::print("attributes: {}\n", name);
    }

    // PROGRAM_UNIFORMS.
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
    for(int unif = 0; unif < numActiveUniforms; ++unif)
    {
        glGetProgramResourceiv(prog, GL_UNIFORM, unif, properties.size(),
        &properties[0], values.size(), NULL, &values[0]);

        name_data.resize(values[0]); //The length of the name.
        glGetProgramResourceName(prog, GL_UNIFORM, unif, name_data.size(), NULL, &name_data[0]);
        std::string name(name_data.begin(), name_data.end());
        fmt::print("uniform: {}\n", name);
    }
}


bool graphics::link_success(uint32_t program_id)
{
    int gl_params = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &gl_params);
    return gl_params;
}

//static
uint32_t graphics::shader_type_from_extension(const std::string& filename)
{
    std::string_view view(filename);
    view = view.substr(view.find_last_of(".") + 1);

    if (view == "vertex")
        return GL_VERTEX_SHADER;
    else if (view == "fragment")
        return GL_FRAGMENT_SHADER;
    else if (view == "geometry")
        return GL_GEOMETRY_SHADER;
    else if (view == "tess_control")
        return GL_TESS_CONTROL_SHADER;
    else if (view == "tess_eval")
        return GL_TESS_EVALUATION_SHADER;
    // else if (view == "compute")
    //     return GL_COMPUTE_SHADER;
    else
        return 0;
}


void graphics::reload_shaders(uint32_t& program)
{
    //@Incomplete: either know which shaders to reload or reload all shaders
    // associated with this program.
    std::string vertex_shader  = "assets/shaders/text.vertex";
    std::string fragment_shader = "assets/shaders/text.fragment";
    glDeleteProgram(program);

    program = glCreateProgram();
    uint32_t vertex   = graphics::load_compile_attach_shader(program, vertex_shader);
    uint32_t fragment = graphics::load_compile_attach_shader(program, "assets/shaders/text.fragment");
    glLinkProgram(program);
    glDetachShader(program, vertex);
    glDetachShader(program, fragment);

    // unlink the shader from the program?
    // deleteShader
    // load_compile_attach_shader?
}



void graphics::render_2d_left_handed_dc(const uint32_t active_shader)
{
    const uint32_t window_height = globals.window_height;
    const uint32_t window_width = globals.window_width;
    const float top   = window_height; // viewport 
    const float bot   = 0.0f;
    const float left  = 0.0f;
    const float right = window_width; // viewport
    const float z_near  = 0.0f;
    const float z_far   = 1.0f; // near and far are reserved by windows???

    //@Note: do NOT forget this.
    const bool row_major = true;
    Mat4 projection_matrix = mat::ortho(left, right, top, bot, z_near, z_far); 
    glUniformMatrix4fv(glGetUniformLocation(graphics::shaders()["text"], "projection_matrix"), 1, row_major, &projection_matrix[0][0]);
}

void graphics::render_3d_left_handed_perspective(const uint32_t active_shader)
{
    //@Note: do NOT forget this.
    const bool row_major = true;

    //@Note: view matrix is identity (i.e. opengl_left_handed, vec3(0.0,0.0,0.)
    Vec3 camera_position = {0};
    Vec3 target_position = {0.0f, 0.0f, -1.0f};
    Vec3 up_vector{0.0f, 1.0f, 0.0f};
    Mat4 view_matrix = mat::view(camera_position, target_position, up_vector);
    // @Note: near_z and far_z should be positive in this context.
    const float fov_in_degrees     = 90.0f;
    const float perspective_near_z = 0.1f;
    const float perspective_far_z  = 100.0f;
    const float aspect_ratio = static_cast<float>(globals.window_width) / static_cast<float>(globals.window_height);
    Mat4 projection_matrix = mat::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);

    // bind view matrix
    const int32_t view_matrix_location = glGetUniformLocation(active_shader, "view_matrix");
    glUniformMatrix4fv(view_matrix_location, 1, row_major, &view_matrix[0][0]);

    // bind projection matrix.
    const int32_t projection_matrix_location = glGetUniformLocation(active_shader, "projection_matrix");
    glUniformMatrix4fv(projection_matrix_location, 1, row_major, &projection_matrix[0][0]);

}


std::map<std::string, uint32_t>& graphics::shaders()
{
    static std::map<std::string, uint32_t> shaders;
    return shaders;
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