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

#include "../file/file.h"

void graphics::init_graphics()
{ 
    gl_lite_init();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);
    
    //@Refactor::for font. should we move it there?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    graphics::Shaders& shader_programs = graphics::shaders();
    //@Refactor: default init these to 0, so we can verify whether they are 0 in reload_shaders?
    shader_programs.default = glCreateProgram();
    shader_programs.text    = glCreateProgram();
    shader_programs.normals = glCreateProgram(); 
    shader_programs.bomb    = glCreateProgram();

    //@Refactor: use reload shaders?
    uint32_t text_vertex   = graphics::load_compile_attach_shader(shader_programs.text, "../shaders/text.vertex");
    uint32_t text_fragment = graphics::load_compile_attach_shader(shader_programs.text, "../shaders/text.fragment");
    glLinkProgram(shader_programs.text);
    glDetachShader(shader_programs.text, text_vertex);
    glDetachShader(shader_programs.text, text_fragment);

    uint32_t normal_vertex   = graphics::load_compile_attach_shader(shader_programs.normals, "../shaders/normals.vertex");
    uint32_t normal_fragment = graphics::load_compile_attach_shader(shader_programs.normals, "../shaders/normals.fragment");
    glLinkProgram(shader_programs.normals);
    glDetachShader(shader_programs.normals, normal_vertex);
    glDetachShader(shader_programs.normals, normal_fragment);

    set_shader(graphics::Shader_Type::SHADER_TEXT);
}

void graphics::reload_shaders(uint32_t& program)
{
    std::string vertex_shader  = "../shaders/text.vertex";
    std::string fragment_shader = "../shaders/text.fragment";
    glDeleteProgram(program);

    program = glCreateProgram();
    uint32_t vertex   = graphics::load_compile_attach_shader(program, vertex_shader);
    uint32_t fragment = graphics::load_compile_attach_shader(program, "../shaders/text.fragment");
    glLinkProgram(program);
    glDetachShader(program, vertex);
    glDetachShader(program, fragment);

    // unlink the shader from the program?
    // deleteShader
    // load_compile_attach_shader?
}

graphics::Shaders& graphics::shaders()
{
    static Shaders shaders;
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

void graphics::set_shader(Shader_Type shader_type)
{
    // using namespace graphics;

    graphics::Shaders& shader_programs = graphics::shaders();
    if (shader_type == Shader_Type::SHADER_TEXT)
    {
        glUseProgram(shader_programs.text);
    }
    else if(shader_type == Shader_Type::SHADER_BOMB)
    {
        glUseProgram(shader_programs.bomb);
    }
    else if(shader_type == Shader_Type::SHADER_DEFAULT)
    {
        glUseProgram(shader_programs.default);
    }
    else if(shader_type == Shader_Type::SHADER_NORMALS)
    {
        glUseProgram(shader_programs.normals);
    }
}


void graphics::generate_vertices_from_raw_data(graphics::Raw_Obj_Data& raw_data)
{
    // preallocate space.
    raw_data.vertices.resize(raw_data.faces.size());

    for (auto &face : raw_data.faces)
    {
        for (auto& index_set : face.indices_set)
        {
            graphics::Vertex temp = { 
                            raw_data.positions[index_set.data[0]].x,  //vx
                            raw_data.positions[index_set.data[0]].y,  //vy
                            raw_data.positions[index_set.data[0]].z,  //vz
                            raw_data.tex_coords[index_set.data[1]].u, //u
                            raw_data.tex_coords[index_set.data[1]].v, //v     
                            raw_data.normals[index_set.data[2]].x,    //nx
                            raw_data.normals[index_set.data[2]].y,    //ny
                            raw_data.normals[index_set.data[2]].z,    //nz     
                          };
            raw_data.vertices.emplace_back(temp);
        }
    }
}


// Everything happens in here. I need to think about what to separate to which extent.
void graphics::draw_game_3d()
{

    // draw all buffers?
    // for now, draw the cat.
    set_shader(graphics::Shader_Type::SHADER_NORMALS);
    // // calculate view transformation.


    // // bind light position. not necessary for the normals.
    // uint32_t normal_shader = graphics::shaders().normals;
    // int32_t light_position_location = glGetUniformLocation(normal_shader,light_position);
    // int32_t light_color_location    = glGetUniformLocation(normal_shader,light_color);
    // int32_t material_location       = glGetUniformLocation(normal_shader,material);
    // vec::Vec3 light_position = {0.0f, 0.0f, 0.5f};
    // vec::Vec3 light_color = {1.0f, 1.0f, 1.0f};
    // Vec4 material =  {0.4f, 0.6f, 0.8f, 64f};
    // glUniform3fv(light_position_location, 1, &light_position.data[0]);
    // glUniform3fv(light_color_location, 1, &light_color.data[0])
    // glUniform4fv(material_location, 1, material.data());

    // bind the view matrix to the uniform. 
    int32_t view_matrix_location = glGetUniformLocation(normal_shader, view_matrix);
    view_matrix = view_scale_matrix * view_rotation_matrix * view_translation_matrix;
    glUniformMatrix4fv(view_matrix_location,      1, false, d_viewMatrix.data());
    //use the projection matrix, set in the beginning:


    Mat4 projection_matrix = mmat::perspective(75, aspect_ratio, 0.1f, 1.0f);    
    glUniformMatrix4fv(d_projectionMatrixLocation,      1, false, d_projectionMatrix.data());

    // // ???????
    // glActiveTexture(GL_TEXTURE0);
    // //calculate object model matrix
    // //        object.modelMatrix = object.translationMatrix * object.rotationMatrix * object.scaleMatrix;
    // object.modelMatrix = object.scaleMatrix * object.rotationMatrix * object.translationMatrix;
    // object.normalTransformMatrix = object.modelMatrix.normalMatrix();

    // glUniformMatrix4fv(d_modelMatrixLocation, 1, false, object.modelMatrix.data());
    // glUniformMatrix3fv(d_normalTransformMatrixLocation, 1, false, object.normalTransformMatrix.data());

    // glBindVertexArray(gVAO);
    // glBindTexture(GL_TEXTURE_2D, object.TBO);

    // glUniform1i(d_textureLocation, 0);
    // glDrawArrays(GL_TRIANGLES,0, object.interleaved_vertices.size()); //changed
    // ///

    // glUseProgram(0); // NULL?





    // GLuint IBO;
    // gluint normal_VAO;
    // gluint position_VAO;
    // gluint texture_coord_VAO;
    // glGenBuffers(1, &IBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // // glActiveTexture(GL_TEXTURE0);




    // auto& raw_data = graphics::cat_data();
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, raw_data.faces.size() * sizeof(graphics::Face), raw_data.data(), GL_STATIC_DRAW);



}



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

uint32_t graphics::load_compile_attach_shader(uint32_t program, std::string file_name)
{

    // set shader type based on the extension. maybe change shader_type to take const char*
    std::string filename = file_name;
    GLenum shader_type = shader_type_from_extension(filename);
    if (shader_type == 0)
        fmt::print("incorrect shader type.\n");
    //set Shader
    GLuint shader_id = glCreateShader(shader_type);
    if (shader_id == 0)
        fmt::print("[graphics] glCreateShader failed.\n");
    else
        fmt::print("[graphics] glCreateShader succeeded. created shader ID {}\n", shader_id);

    //@Cleanup:to const char* buffer?
    std::string target = {};
    file::file_to_string(filename, target);
    const char* c_str = target.c_str();

    glShaderSource(shader_id, 1, &c_str, NULL);
    glCompileShader(shader_id);

    GLint shader_compiled = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);

    if (shader_compiled != GL_TRUE)
    {
        fmt::print("[graphics] shader_from_file: unable to compile shader {}\n", filename);
        GLint max_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        std::vector<GLchar> error_log(max_length);
        glGetShaderInfoLog(shader_id, max_length, &max_length, &error_log[0]);

        // Provide the infolog in whatever manor you deem best.
        auto string_log = std::string(error_log.begin(), error_log.end());
        fmt::print("[graphics] shader error log: {}\n", string_log);
        // Exit with failure.
        glDeleteShader(shader_id); // Don't leak the shader.
        // printShaderLog(shader);
    }
    else
    {
        fmt::print("[graphics] shader_from_file: successfully compiled {}\n", filename);
        glAttachShader(program, shader_id);
        // i think this is causing the problem?
        // glLinkProgram(program);
        // glDetachShader(program, shader_id); // can also postpone, but lower memory footprint
        // this way.
    }
    return shader_id;
}


//@Refactor:    
// we need a null-terminated bytestring in order to use sscanf.
// we can point to an address in memory, but then it doesn't null terminate.
// read until either linebreak or EOF. replace linebreak by \0.
// hand ptr over to user.
// read line by line 
// size_t file_size = file::get_file_size(filename);
// if (file_size == 0)
//     fmt::print("load_obj: could not read file {}", filename);
// hand me a pointer, and I will alloc for you.
// we can mitigate the unsafety of sscanf by limiting the number of characters that are 
// read by %s by affixing a number, i.e. %1s for 1 character only.  
void graphics::load_obj(const std::string& filename, graphics::Raw_Obj_Data& raw_data)
{
    std::string data ={};
    // we assume the filename to be valid?
    file::file_to_string(filename, data);
    std::stringstream data_stream(data);
    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[20] = {}; // used for the garbage in each line. 

    raw_data.positions.resize(4000);
    raw_data.normals.resize(4000);
    raw_data.tex_coords.resize(4000);
    raw_data.faces.resize(4000);

    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
        ++line_number;
        if (line[0] == 's')
        {
            // what does that mean?
            continue;
        }
        else if (line[0] == '#') // comment 
        {
            continue;
        }
        else if (line[0] ==  'v' && line[1] == ' ') // vertex
        {
            vec::Vec3 pos = {};
            sscanf(line.c_str(), "%9s %f %f %f", garbage_buffer, &pos.x, &pos.y, &pos.z);
            raw_data.positions.emplace_back(pos);
        } 
        else if (line[0] == 'v' && line[1] == 't') // texture coordinates
        {
            Vec2 tex_coords = {};
            sscanf(line.c_str(), "%9s %f %f", garbage_buffer, &tex_coords.x, &tex_coords.y);
            raw_data.tex_coords.emplace_back(tex_coords);
        }
        else if (line[0] == 'v' && line[1] == 'n') // vertex normals
        {
            vec::Vec3 normal = {};
            sscanf(line.c_str(), "%9s %f %f %f", garbage_buffer, &normal.x, &normal.y, &normal.z);
            raw_data.normals.emplace_back(normal);
        }
        else if (line[0] == 'f') // face indices
        {
            Face face = {};

            //@incomplete: This will barf on unstructured obj files. for now, we assume everything's present.
            sscanf(line.c_str(), "%9s %u / %u / %u %u /%u /%u %u /%u /%u",
                   garbage_buffer,
                   &face.indices_set[0].data[0], &face.indices_set[0].data[1], &face.indices_set[0].data[2],
                   &face.indices_set[1].data[0], &face.indices_set[1].data[1], &face.indices_set[1].data[2],
                   &face.indices_set[2].data[0], &face.indices_set[2].data[1], &face.indices_set[2].data[2]
                   );

            // The indices in the wavefront obj start at 1. we offset them to use them correctly
            // with the arrays in raw_data.
            face.indices_set[0] -= 1;
            face.indices_set[1] -= 1;
            face.indices_set[2] -= 1;

            raw_data.faces.emplace_back(face);
        }
        else
            fmt::print("[graphics] ERROR: load_obj: no matching indicator. line number: {}. content: {}\n", line_number, line);
    }
    fmt::print("[graphics] succesfully loaded {}. num_faces: {}\n", filename, raw_data.faces.size());
}

//@Temporary.
graphics::Raw_Obj_Data& graphics::cat_data()
{
    static graphics::Raw_Obj_Data cat_data;
    return cat_data;
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
