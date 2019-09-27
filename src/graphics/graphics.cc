#include "graphics.h"
#undef max
#undef min
#include <fmt/core.h>
#include <stdint.h>
// #include <GL/GL.h>

#include "gl_lite.h"

#include <Wingdi.h>
#include <stdlib.h>
#include <string>
// #define TINYOBJLOADER_IMPLEMENTATION
// #include "tinyobjloader.h"
#include "../file/file.h"


void graphics::init_opengl()
{ 
    gl_lite_init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

    graphics::Shaders& shader_programs = graphics::shaders();

    shader_programs.default_shader_program = glCreateProgram();
    shader_programs.text_shader_program    = glCreateProgram();
    shader_programs.normals_shader_program = glCreateProgram(); 
    shader_programs.bomb_shader_program    = glCreateProgram();

    // graphics::load_compile_attach_shader(shader_programs.normals_shader_program,"../../resources/shaders/normal.vertex");
    // graphics::load_compile_attach_shader(shader_programs.normals_shader_program,"../../resources/shaders/normal.fragment");

    graphics::load_compile_attach_shader(shader_programs.text_shader_program, "../shaders/text.vertex");
    graphics::load_compile_attach_shader(shader_programs.text_shader_program, "../shaders/text.fragment");

    set_shader(graphics::Shader_Type::SHADER_TEXT);
}

graphics::Shaders& graphics::shaders()
{
    static Shaders shaders;
    return shaders;
}

graphics::Win32_Context& graphics::global_Win32_context()
{
	static Win32_Context context;
	return context;
}

void graphics::set_shader(Shader_Type shader_type)
{
    // using namespace graphics;

    graphics::Shaders& shader_programs = graphics::shaders();
    if (shader_type == Shader_Type::SHADER_TEXT)
    {
        glUseProgram(shader_programs.text_shader_program);
    }
    else if(shader_type == Shader_Type::SHADER_BOMB)
    {
        glUseProgram(shader_programs.bomb_shader_program);
    }
    else if(shader_type == Shader_Type::SHADER_DEFAULT)
    {
        glUseProgram(shader_programs.default_shader_program);
    }
    else if(shader_type == Shader_Type::SHADER_NORMALS)
    {
        glUseProgram(shader_programs.normals_shader_program);
    }
}


void graphics::draw_game_3d()
{
    // draw all buffers?
}

void graphics::clear_buffers()
{
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics::render_frame()
{
    graphics::draw_game_3d();
    graphics::swap_buffers();
}

void graphics::swap_buffers()
{
	SwapBuffers(graphics::global_Win32_context().device_context);
}

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
    else if (view == "tess_evaluation")
        return GL_TESS_EVALUATION_SHADER;
    // else if (view == "compute")
    //     return GL_COMPUTE_SHADER;
    else
        return 0;
}


void graphics::get_shader_info(uint32_t prog)
{
    // GLint numActiveAttribs = 0;
    GLint numActiveUniforms = 0;
    // glGetProgramInterfaceiv(prog, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
    glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
    std::vector<GLchar> nameData(256);
    std::vector<GLenum> properties;
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    std::vector<GLint> values(properties.size());
    // for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
    // {
    //     glGetProgramResourceiv(prog, GL_PROGRAM_INPUT, attrib, properties.size(),
    //     &properties[0], values.size(), NULL, &values[0]);

    //     nameData.resize(values[0]); //The length of the name.
    //     glGetProgramResourceName(prog, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
    //     std::string name((char*)&nameData[0], nameData.size() - 1);
    // }

    for(int unif = 0; unif < numActiveUniforms; ++unif)
    {
        glGetProgramResourceiv(prog, GL_UNIFORM, unif, properties.size(),
        &properties[0], values.size(), NULL, &values[0]);

        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(prog, GL_UNIFORM, unif, nameData.size(), NULL, &nameData[0]);
        std::string name((char*)&nameData[0], nameData.size() - 1);
        fmt::print("uniform: {}\n", name);
    }



}


bool graphics::load_compile_attach_shader(uint32_t program, const char* file_name)
{
    bool success = true;
    // set shader type based on the extension. maybe change shader_type to take const char*
    std::string filename = file_name;
    GLenum shader_type = shader_type_from_extension(filename);

    //set Shader
    GLuint shader_id = glCreateShader(shader_type);
    if (shader_id == 0)
        fmt::print("glCreateShader borked. sigh.");
    else
        fmt::print("glCreateShader succeeded. created shader ID {}", shader_id);

    //@cleanup: maybe directly transfer to string?
    std::string target = {};
    file::file_to_string(filename, target);
    const char* c_str = target.c_str();

    glShaderSource(shader_id, 1, &c_str, NULL);

    glCompileShader(shader_id);


    GLint shader_compiled = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);

    if (shader_compiled != GL_TRUE)
    {
        success = false;
        fmt::print("shader_from_file: unable to compile vertex shader {}\n", filename);
        GLint max_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(shader_id, max_length, &max_length, &error_log[0]);

        // Provide the infolog in whatever manor you deem best.

        // Exit with failure.
        glDeleteShader(shader_id); // Don't leak the shader.
        // printShaderLog(shader);
    }
    else
    {
        fmt::print("shader_from_file: successfully compiled {}\n", filename);
        glAttachShader(program, shader_id);
        glLinkProgram(program);
        glDetachShader(program, shader_id); // can also postpone, but lower memory footprint
        // this way.
        success = true;
    }


    return success;
}

static void load_obj(const char* filename)
{
    // std::string inputfile = filename;
    // tinyobj::attrib_t attrib;
    // std::vector<tinyobj::shape_t> shapes;
    // std::vector<tinyobj::material_t> materials;

    // std::string warning;
    // std::string error;

    // bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

    // if (!warn.empty())
    //     fmt::print("tinyobj warning: {}", warning);

    // if (!error.empty())
    //     fmt::print("tinyobj error: {}", error);

    // if (!ret)
    //     fmt::print("tinyobj:: something went horribly wrong.");

}




// static void draw_menu()
// {
//     // tell opengl how we want to render
//     rendering_2d_right_handed_unit_scale();
//     set_render_target(0, hdr_buffer);
//     set_depth_target(0, hdr_depth);

//     glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     resolve_to_ldr();

//     set_shader(shader_text);

//     font::Font text_font = font::get_font_at_size(font_folder, "font_name", font_size);
//     std::string text = {"hello there"};
//     draw_text(text_font, 200, 200, {0, 1, 1}, text,  )
// }

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

//  graphics::swap_buffers();

// }