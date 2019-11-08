#include "graphics.h"
#undef max
#undef min
#include <fmt/core.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include "gl_lite.h"
#include <Wingdi.h>
#include <stdlib.h>

#define BUFFER_OFFSET(i) ((void*)(i)) //hacky macro for offset.

#include "../on_leaving_scope/on_leaving_scope.h"
#include "../file/file.h"
#include "../mat/mat.h"
#include "../mat4/mat4.h"
#include "../mat3/mat3.h" // for normal matrix.

static uint32_t VBO = 0;
static uint32_t VAO = 0;

void graphics::init_graphics()
{ 
    // init gl_lite only after the gl_context has been created.
    gl_lite_init();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //@Refactor: These are used for font. should we move it there?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //@Refactor: default init these to 0, so we can verify whether they are 0 in reload_shaders?
    graphics::Shaders& shader_programs = graphics::shaders();
    shader_programs.default = glCreateProgram();
    shader_programs.text    = glCreateProgram();
    shader_programs.normals = glCreateProgram(); 
    shader_programs.bomb    = glCreateProgram();

    //@Refactor: use reload shaders?
    uint32_t text_vertex   = graphics::load_compile_attach_shader(shader_programs.text, "assets/shaders/text.vertex");
    uint32_t text_fragment = graphics::load_compile_attach_shader(shader_programs.text, "assets/shaders/text.fragment");
    glLinkProgram(shader_programs.text);
    glDetachShader(shader_programs.text, text_vertex);
    glDetachShader(shader_programs.text, text_fragment);

    // normals
    uint32_t normal_vertex   = graphics::load_compile_attach_shader(shader_programs.normals, "assets/shaders/normals.vertex");
    uint32_t normal_fragment = graphics::load_compile_attach_shader(shader_programs.normals, "assets/shaders/normals.fragment");
    glLinkProgram(shader_programs.normals);
    glDetachShader(shader_programs.normals, normal_vertex);
    glDetachShader(shader_programs.normals, normal_fragment);

    // gouraud
    uint32_t gouraud_vertex   = graphics::load_compile_attach_shader(shader_programs.normals, "assets/shaders/gouraud.vertex");
    uint32_t gouraud_fragment = graphics::load_compile_attach_shader(shader_programs.normals, "assets/shaders/gouraud.fragment");
    glLinkProgram(shader_programs.gouraud);
    glDetachShader(shader_programs.gouraud, gouraud_vertex);
    glDetachShader(shader_programs.gouraud, gouraud_fragment);


    //@refactor: we should abstract this. But I'm not quite sure how yet.
    const int buffer_count = 1;
    glGenBuffers(buffer_count, &VBO);     
    glGenVertexArrays(buffer_count, &VAO); 
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    uint32_t pos_array = 0;
    uint32_t uv_array = 1;
    uint32_t normals_array = 2;
    glEnableVertexAttribArray(pos_array);
    glEnableVertexAttribArray(uv_array);
    glEnableVertexAttribArray(normals_array);
    glVertexAttribPointer(pos_array,     3, GL_FLOAT, GL_FALSE, sizeof(asset::Vertex), 0); // x, y, z
    glVertexAttribPointer(uv_array,      2, GL_FLOAT, GL_FALSE, sizeof(asset::Vertex), BUFFER_OFFSET(3 * sizeof(float))); // skip  3: u, v,
    glVertexAttribPointer(normals_array, 3, GL_FLOAT, GL_FALSE, sizeof(asset::Vertex), BUFFER_OFFSET(5 * sizeof(float))); // skip 5: nx, ny, nz.

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    graphics::set_shader(graphics::Shader_Type::SHADER_DEFAULT);
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

void graphics::set_shader(Shader_Type shader_type)
{
    graphics::Shaders& shader_programs = graphics::shaders();
    if (shader_type == Shader_Type::SHADER_TEXT)
    {
        glUseProgram(shader_programs.text);
    }
    else if (shader_type == Shader_Type::SHADER_BOMB)
    {
        glUseProgram(shader_programs.bomb);
    }
    else if (shader_type == Shader_Type::SHADER_DEFAULT)
    {
        glUseProgram(shader_programs.default);
    }
    else if (shader_type == Shader_Type::SHADER_GOURAUD)
    {
        glUseProgram(shader_programs.gouraud);
    }
    else if (shader_type == Shader_Type::SHADER_NORMALS)
    {
        glUseProgram(shader_programs.normals);
    }
}


// Everything happens in here. I need to think about what to separate to which extent.
void graphics::draw_game_3d()
{
    graphics::set_shader(graphics::Shader_Type::SHADER_NORMALS);
    uint32_t active_shader = graphics::shaders().normals;
    uint32_t normal_shader = graphics::shaders().normals;

    auto defer_shader_state = On_Leaving_Scope([]{set_shader(graphics::Shader_Type::SHADER_DEFAULT);});

    // matrices:
    const bool row_major = true;
    // View matrix:
    int32_t view_matrix_location = glGetUniformLocation(normal_shader, "view_matrix");
    Mat4 view_scale_matrix       = mat::mat4_identity();
    Mat4 view_rotation_matrix    = mat::mat4_identity();
    Mat4 view_translation_matrix = mat::mat4_identity();
    Mat4 view_matrix = view_scale_matrix * view_rotation_matrix * view_translation_matrix;
    glUniformMatrix4fv(view_matrix_location, 1, row_major, &view_matrix[0][0]);

    // Projection Matrix:
    // perspective near_z and far_z define the clipping, not the actual bounds. I think.
    // @Note: Then what should far_z and near_z be?
    int32_t projection_matrix_location = glGetUniformLocation(normal_shader, "model_projection");
    const float fov_in_degrees = 90.0f;
    const float perspective_near_z = 0.1f;
    const float perspective_far_z = 10.0f;
    const float aspect_ratio = graphics::window_settings().width / graphics::window_settings().height;
    Mat4 projection_matrix = mat::perspective(fov_in_degrees, aspect_ratio, perspective_near_z, perspective_far_z);
    glUniformMatrix4fv(projection_matrix_location, 1, row_major, &projection_matrix[0][0]);




    // bind the VAO before the VBO.
    glBindVertexArray(VAO);
    //On_Scope_Exit(GlBindVertexArray(0));
    // bind the VBO buffer to array buffer.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
     //On_Scope_Exit(glBindBuffer(GL_ARRAY_BUFFER, 0));
    // bind vertex array object.


    // for each object in the active scene:

        // Model Matrix:
        int32_t model_matrix_location = glGetUniformLocation(normal_shader, "model_matrix");
        Xform_State cat_state = {};
        cat_state.position = {0.0f, 0.0f, -1.2f};
        cat_state.q_orientation = {0.0f, 0.0f, 0.0f, 1.0f};
        cat_state.scale = 0.1f;

        Mat4 model_matrix = mat::mat4_from_xform_state(cat_state);
        glUniformMatrix4fv(model_matrix_location, 1, row_major, &model_matrix[0][0]);

        int32_t normal_transform_matrix_location = glGetUniformLocation(normal_shader, "normal_transform");
        Mat3 normal_transform_matrix = mat::normal_transform(model_matrix);
        //@Note: We need to actually verify whether or not this is transposed.
        glUniformMatrix3fv(normal_transform_matrix_location, 1, row_major, &normal_transform_matrix[0][0]);

        auto& cat_data = graphics::cat_data();
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<int>(cat_data.vertices.size() * sizeof(asset::Vertex)),
                     cat_data.vertices.data(),
                     GL_STATIC_DRAW);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, object.TBO);
        // glUniform1i(d_textureLocation, 0);
        // glUseProgram(0); // NULL?
    // end for

    // for each light in the active scene:
        // lights are a property of the scene. 
        // however, this implies that the properties of the scene
        // are bound to uniforms in openGL. I don't necessarily think
        // that that is a good idea. Or do we insert an array of lights to openGL?
        // is there an uniform array?

        // bind light position. not necessary for the normals, but necessary for every other shader.
        // uint32_t normal_shader = graphics::shaders().normals;
        // int32_t light_position_location = glGetUniformLocation(normal_shader,light_position);
        // int32_t light_color_location    = glGetUniformLocation(normal_shader,light_color);
        // int32_t material_location       = glGetUniformLocation(normal_shader,material);
        // Vec3 light_position = {0.0f, 0.0f, 0.5f};
        // Vec3 light_color =    {1.0f, 1.0f, 1.0f};
        // Vec4 material =  {0.4f, 0.6f, 0.8f, 64f};
        // glUniform3fv(light_position_location, 1, &light_position.data[0]);
        // glUniform3fv(light_color_location, 1, &light_color.data[0])
        // glUniform4fv(material_location, 1, material.data());
    // end for


    // actually draw.
    glDrawArrays(GL_TRIANGLES,0, cat_data.vertices.size());

}

// static
void graphics::clear_buffers()
{
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClear(GL_DEPTH_BUFFER_BIT);
}

void graphics::render_frame()
{
    graphics::draw_game_3d();
}

void graphics::swap_buffers()
{
	SwapBuffers(graphics::global_Win32_context().device_context);
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
    else if (view == "tess_evaluation")
        return GL_TESS_EVALUATION_SHADER;
    // else if (view == "compute")
    //     return GL_COMPUTE_SHADER;
    else
        return 0;
}

//@TODO:turn this into multiple things? i.e. get_shader_info,.
// get_program_info, get_opengl_state.
// static
void graphics::get_shader_info(uint32_t prog)
{

    fmt::print("shader info for program {}:\n", prog);

    std::vector<GLchar> nameData(256);
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

        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(prog, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
        std::string name((char*)&nameData[0], nameData.size() - 1);
        fmt::print("attributes: {}\n", name);

    }
    // PROGRAM_UNIFORMS.
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
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

// returns the shader_id.
uint32_t graphics::load_compile_attach_shader(uint32_t program, std::string file_name)
{
    // set shader type based on the extension. 
    std::string filename = file_name;
    GLenum shader_type = shader_type_from_extension(filename);
    if (shader_type == 0)
        fmt::print("[graphics] incorrect shader type.\n");
    //set Shader
    uint32_t shader_id = glCreateShader(shader_type);
    if (shader_id == 0)
        fmt::print("[graphics] glCreateShader failed.\n");
    else{
        fmt::print("[graphics] glCreateShader succeeded. created shader ID {}\n", shader_id);
    }

    //@Cleanup:to const char* buffer?
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

        //  maxLength includes the NULL character.
        std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(shader_id, max_length, &max_length, &error_log[0]);
        // Provide the infolog in whatever manner you deem best.
        auto string_log = std::string(error_log.begin(), error_log.end());
        fmt::print("[graphics] shader error log: {}\n", string_log);
        // Exit with failure.
        glDeleteShader(shader_id); // Don't leak the shader.
    }
    else
    {
        fmt::print("[graphics] shader_from_file: successfully compiled {}\n", filename);
        glAttachShader(program, shader_id);
    }
    return shader_id;
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

//@Temporary.
asset::Raw_Obj_Data& graphics::cat_data()
{
    static asset::Raw_Obj_Data cat_data;
    return cat_data;
}

graphics::Shaders& graphics::shaders()
{
    static Shaders shaders = {};
    return shaders;
}

graphics::Window_Settings& graphics::window_settings()
{
    static Window_Settings settings;
    return settings;
}


graphics::Win32_Context& graphics::global_Win32_context() //@cleanup: i don't like this.
{
    static Win32_Context context;
    return context;
}
