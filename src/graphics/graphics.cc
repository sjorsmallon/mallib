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

    const int buffer_count = 1;
    glGenBuffers(buffer_count, &VBO);     // glsizei n, GLuint *buffers
    glGenVertexArrays(buffer_count, &VAO); // glsizei n, GLuinbt *arrays

    set_shader(graphics::Shader_Type::SHADER_DEFAULT);
}

void graphics::reload_shaders(uint32_t& program)
{
    //@Incomplete: either know which shaders to reload or reload all shaders
    // associated with this program.
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

void graphics::set_shader(Shader_Type shader_type)
{
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
    //@Memory: preallocate space.
    for (auto &position: raw_data.positions)
    {
    }
    raw_data.vertices.resize(raw_data.faces.size());


    for (auto &face : raw_data.faces)
    {
        //@Refactor: maybe rewrite face to use the index set?

            graphics::Vertex v0 = {
                raw_data.positions[face.v0_indices.data[0]],
                raw_data.tex_coords[face.v0_indices.data[1]],
                raw_data.normals[face.v0_indices.data[2]]
            };

            graphics::Vertex v1 = {
                raw_data.positions[face.v1_indices.data[0]],
                raw_data.tex_coords[face.v1_indices.data[1]],
                raw_data.normals[face.v1_indices.data[2]]
            };

            graphics::Vertex v2 = {
                raw_data.positions[face.v2_indices.data[0]],
                raw_data.tex_coords[face.v2_indices.data[1]],
                raw_data.normals[face.v2_indices.data[2]]
            };

            raw_data.vertices.emplace_back(v0);
            raw_data.vertices.emplace_back(v1);
            raw_data.vertices.emplace_back(v2);
    }

}


// Everything happens in here. I need to think about what to separate to which extent.
void graphics::draw_game_3d()
{
    // draw all buffers?
    // for now, draw the cat.
    set_shader(graphics::Shader_Type::SHADER_NORMALS);
    uint32_t active_shader = graphics::shaders().normals;
    auto defer_shader_state = On_Leaving_Scope([]{set_shader(graphics::Shader_Type::SHADER_DEFAULT);});
   

    // lights are a property of the scene. 
    // however, this implies that the properties of the scene
    // are bound to uniforms in openGL. I don't necessarily think
    // that that is a good idea. Or do we insert an array of lights to openGL?
    // is there an uniform array?

    // bind light position. not necessary for the normals.
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

    uint32_t normal_shader = graphics::shaders().normals;

    // matrices:
    const bool row_major = true;

    // View matrix:
    int32_t view_matrix_location = glGetUniformLocation(normal_shader, "view_matrix");
    Mat4 view_scale_matrix = mat::mat4_identity();
    Mat4 view_rotation_matrix = mat::mat4_identity();
    Mat4 view_translation_matrix = mat::mat4_identity();
    Mat4 view_matrix = view_scale_matrix * view_rotation_matrix * view_translation_matrix;

    glUniformMatrix4fv(view_matrix_location, 1, row_major, &view_matrix[0][0]);

    // Projection Matrix:
    int32_t projection_matrix_location = glGetUniformLocation(normal_shader, "model_projection");
    const float fov = 75.0f;
    const float perspective_near_z = 0.1f;
    const float perspective_far_z = 1.0f;
    const float aspect_ratio = graphics::window_settings().width / graphics::window_settings().height;
    Mat4 projection_matrix = mat::perspective(fov, aspect_ratio, perspective_near_z, perspective_far_z);    
    glUniformMatrix4fv(projection_matrix_location, 1, row_major, &projection_matrix[0][0]);



    // Model Matrix:
    int32_t model_matrix_location = glGetUniformLocation(normal_shader, "model_matrix");
    Xform_State cat_state = {};
    cat_state.position = {0.0f, 0.0f, -0.8f};
    cat_state.q_orientation = {0.0f, 0.0f, 0.0f, 1.0f};
    cat_state.scale = 0.2f; 
    //@Refactor: this goes to mat-> to mat4 (from_quat) -> to xform_state(quaternion).
    Mat4 model_matrix = mat::from_xform_state(cat_state);
    glUniformMatrix4fv(model_matrix_location, 1, false, &model_matrix[0][0]);

    int32_t normal_transform_matrix_location = glGetUniformLocation(normal_shader, "normal_transform");
    Mat3 normal_transform_matrix = mat::normal_transform(model_matrix);
    //@Note: We need to actually verify whether or not this is transposed.
    glUniformMatrix3fv(normal_transform_matrix_location, 1, false, &normal_transform_matrix[0][0]);


    //@TODO: write mat::normal_transform(). with all the above uniforms bound, we still need to send the cat's data to the gpu.
    // see how I do it in the ECS2 example.


    //@REFACTOR: we only need to do this once.
 
    // bind the VAO before the VBO.
    glBindVertexArray(VAO);
    //On_Scope_Exit(GlBindVertexArray(0));

    // bind the VBO buffer to array buffer.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    auto& cat_data = graphics::cat_data();
    for (auto& vertex: cat_data.vertices)
    {
        fmt::print("x: {}, y: {}, z: {}", vertex.position.x, vertex.position.y, vertex.position.z);
    }
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<int>(cat_data.vertices.size() * sizeof(graphics::Vertex)),
                 cat_data.vertices.data(),
                 GL_STATIC_DRAW);
    //On_Scope_Exit(glBindBuffer(GL_ARRAY_BUFFER, 0));
    // bind vertex array object.


    //@Refactor: get_next_vertex_attrib_value?
    // struct of three uints?
    uint32_t pos_array = 0;
    uint32_t uv_array = 1;
    uint32_t normals_array = 2;
    glEnableVertexAttribArray(pos_array);
    glEnableVertexAttribArray(uv_array);
    glEnableVertexAttribArray(normals_array);

    //@Refactor: this is modern openGL. do we want to use this?
    // glEnableVertexArrayAttrib(VAO, pos_array);
    // glEnableVertexArrayAttrib(VAO, uv_array);
    // glEnableVertexArrayAttrib(VAO, normals_array);
    
    glVertexAttribPointer(pos_array,     3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // x, y, z
    glVertexAttribPointer(uv_array,      2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float))); // skip  3: u, v,
    glVertexAttribPointer(normals_array, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(5 * sizeof(float))); //skip 5: r, g, b.

    // actually draw.
    glDrawArrays(GL_TRIANGLES,0, cat_data.vertices.size());

    // whahuh?
    // glBindVertexArray(gVAO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, object.TBO);
    // glUniform1i(d_textureLocation, 0);
    // glUseProgram(0); // NULL?
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

uint32_t graphics::load_compile_attach_shader(uint32_t program, std::string file_name)
{
    // set shader type based on the extension. maybe change shader_type to take const char*
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
    }
    else
    {
        fmt::print("[graphics] shader_from_file: successfully compiled {}\n", filename);
        glAttachShader(program, shader_id);
    }
    return shader_id;
}





//@Refactor:    
// we need a null-terminated bytestring in order to use sscanf.
// we can point to an address in memory, but then it doesn't null terminate.
// read until either linebreak or EOF. replace linebreak by \0.
// hand ptr over to user.
// in essence, we want to make our own getline for a char array.
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
    file::file_to_string(filename, data);
    std::stringstream data_stream(data);
    constexpr const int max_string_length = 20;
    constexpr const int max_string_read_length = 9;
    char garbage_buffer[20] = {}; // used for the garbage in each line. 

    // @Memory: how to predict how to preallocate?
    fmt::print("[graphics] Warning: load_obj does not efficiently preallocate.\n");
    raw_data.positions.reserve(4000);
    raw_data.normals.reserve(4000);
    raw_data.tex_coords.reserve(4000);
    raw_data.faces.reserve(4000);

    size_t line_number = 0;
    for (std::string line; std::getline(data_stream, line);)
    {
        ++line_number;
        if (line[0] == '#') // comment 
        {
            continue;
        }
        else if (line[0] == 's')
        {
            // what does the 's' mean?
            continue;
        }
        else if (line[0] ==  'v' && line[1] == ' ') // vertex
        {
            Vec3 pos = {};
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
            Vec3 normal = {};
            sscanf(line.c_str(), "%9s %f %f %f", garbage_buffer, &normal.x, &normal.y, &normal.z);
            raw_data.normals.emplace_back(normal);
        }
        else if (line[0] == 'f') // face indices
        {
            Face face = {};
            fmt::print("{}", line);
            //@incomplete: This will barf on unstructured obj files. for now, we assume everything's present.
            sscanf(line.c_str(), "%9s %u / %u / %u %u /%u /%u %u /%u /%u",
                   garbage_buffer,
                   &face.v0_indices.data[0], &face.v0_indices.data[1], &face.v0_indices.data[2],
                   &face.v1_indices.data[0], &face.v1_indices.data[1], &face.v1_indices.data[2],
                   &face.v2_indices.data[0], &face.v2_indices.data[1], &face.v2_indices.data[2]
                   );

            // The indices in the wavefront obj start at 1. we offset them to use them correctly
            // with the arrays in raw_data.
            //@Note: be careful with this subtraction. made a dumb mistake.
            face.v0_indices -= 1;
            face.v1_indices -= 1;
            face.v2_indices -= 1;
            
            raw_data.faces.emplace_back(face);
        }
        else
            fmt::print("[graphics] ERROR: load_obj: no matching indicator. line number: {}. content: {}\n", line_number, line);
    }
    fmt::print("[graphics] succesfully loaded {}. num_faces: {}\n", filename, raw_data.faces.size());
}

// void graphics::load_mtl(const std::string& filename, graphics::Material& material)
// {
//     std::string data ={};
//     file::file_to_string(filename, data);
//     std::stringstream data_stream(data);
//     constexpr const int max_string_length = 20;
//     constexpr const int max_string_read_length = 9;
//     char garbage_buffer[20] = {}; // used for the garbage in each line. 

//     //@Hack: we assume that if a line starts with "new"
//     // that the line declares a new mtl.
//     // the same is true for 'illum'.

//     // auto mat_vector = std::vector<Material>(1);
//     // Material& active_material = mat_vector[0];

//     size_t line_number = 0;
//     for (std::string line; std::getline(data_stream, line);)
//     {
//         ++line_number;
//         if (line[0] == '#') // comment
//         {
//             continue;
//         }
//         //@TODO: substring lookup instead of this?
//         else if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w')
//         {
//             // the current mtl is done. create a new one and rebind the reference.
//             mat_vector.emplace_back();
//             active_material = mat_vector.back();
//         }
//         else if (line[0] == 'K' && line[1] == 'a') // ambient color
//         {
//             Vec3 color = {};
//             sscanf("%9s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
//         }
//         else if (line[0] == 'K' && line[1] == 'd') // diffuse color
//         {
//             Vec3 color = {};
//             sscanf("%9s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
//         }
//         else if (line[0] == 'K' && line[1] == 's') // specular color
//         {
//             Vec3 color = {};
//             sscanf("%9s %f %f %f", garbage_buffer, &color.r, &color.g, &color.b);
//         }
//         else if (line[0] == 'd') // non_transparency
//         {
//             float alpha = 0;
//             sscanf("%9s %f", &alpha);
//         }
//         else if (line[0] == 'T' && line[1] == 'r') // transparency
//         {
//             float inv_alpha = 0;
//             sscanf("%9s %f", &inv_alpha);

            
//         }
//         else if (line[0] == 'i' && line[1] == 'l') // illumination
//         {
//             int illum = 0;
//             sscanf("%9s %d", &illum);
//             // if illum == 1: we can skip Ks.
//             // if illum == 2: requires ks to be defined.
//         }
//         else if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p') // map_Ka
//         {
//             //@Incomplete:
//         }

//     }

// }


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
