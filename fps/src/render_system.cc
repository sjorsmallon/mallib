#include "render_system.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp> 

#include <set>

#include "camera.h"
#include "logr.h"
#include "light.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "asset_manager.h"
#include "entity_manager.h"
#include "timed_function.h"

constexpr const int NUM_LIGHTS = 32;
constexpr const unsigned int SHADOW_FB_WIDTH = 1024;
constexpr const unsigned int SHADOW_FB_HEIGHT = 1024;
constexpr const float M_PI = 3.1415926535897f;

constexpr const int NONE_VAO = 0;
constexpr const int NONE_VBO = 0;

struct Model_Buffer
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int model_matrix_VBO;    
    std::vector<glm::mat4> model_matrix_buffer;
};

namespace
{
    std::map<std::string, Model_Buffer> f_model_buffers;

    // cvars
    int g_window_width;
    int g_window_height;
    float g_aspect_ratio;

    constexpr const float g_fov = 110.0f;
    constexpr const float g_projection_z_near_plane = 0.1f;
    constexpr const float g_projection_z_far_plane = 2000.0f;

    // "members"
    Shader_Manager*   shader_manager;
    Texture_Manager*  texture_manager;
    Asset_Manager*    asset_manager;
    Entity_Manager*   entity_manager;

    // openGL record keeping

    //--- frame buffers
    unsigned int geometry_fbo; 
    unsigned int depth_rbo;
    //  // shadow
    unsigned int depth_map_fbo;

    //--- textures for framebuffers
    // @IC(Sjors): these are not necessary. They are bound once to the deferred geometry framebuffer.    
    // unsigned int position_tfbo;
    // unsigned int normal_tfbo;
    // unsigned int albedo_specular_tfbo;
    // unsigned int roughness_tfbo;
    // unsigned int metallic_tfbo;


    // unsigned int ambient_occlusion_tfbo;
    // unsigned int 
    // shadow texture buffer
    unsigned int g_depth_map_tfbo;

    //--- uniform buffers
    unsigned int light_ubo;

    // vao / vbos.
    unsigned int g_cube_vao;
    unsigned int g_cube_vbo;

    unsigned int g_quad_vao;
    unsigned int g_quad_vbo;

    unsigned int g_floor_vao;
    unsigned int g_floor_vbo;

    unsigned int g_viewmodel_vao;
    unsigned int g_viewmodel_vbo;
    size_t g_viewmodel_vertex_count;

    unsigned int g_wall_vao;
    unsigned int g_wall_vbo;
    unsigned int g_wall_model_vbo;
    unsigned int g_wall_mvp_vbo;
    std::vector<glm::mat4> g_wall_model_matrices(256);
    std::vector<glm::mat4> g_wall_mvp_matrices(256);

    unsigned int g_debug_geometry_vao;
    unsigned int g_debug_geometry_vbo;
    std::vector<float> g_debug_draw_data;

    unsigned int g_dodecahedron_vao;
    unsigned int g_dodecahedron_vbo;
    unsigned int g_dodecahedron_model_vbo;
    unsigned int g_dodecahedron_mvp_vbo;
    std::vector<glm::mat4> g_dodecahedron_model_matrices(2048);
    std::vector<glm::mat4> g_dodecahedron_mvp_matrices(2048);

    unsigned int g_hud_vao;
    unsigned int g_hud_vbo;

    unsigned int g_parallax_cube_vao;
    unsigned int g_parallax_cube_vbo;
  

    // openGL error callback
    void GLAPIENTRY opengl_message_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam )
    {
        /// these warning IDs are ignored.
        /// 0x8251: bound to video memory (intended.)
        /// 0x8250: buffer performance warning: copying atomic buffer (which we want to do, since we need the values.)
        std::set<GLenum> warning_types_to_ignore{0x8251, 0x8250};

        const bool warning_can_be_ignored = (warning_types_to_ignore.find(type) != warning_types_to_ignore.end());

        if (!warning_can_be_ignored) 
        {
            if (type == GL_DEBUG_TYPE_ERROR) logr::report_error( "GL CALLBACK: type = 0x{:x}, severity = 0x{:x}, message = {}\n", type, severity, message);
            else
            {
                logr::report("GL CALLBACK: type = 0x{:x}, severity = 0x{:x}, message = {}\n", type, severity, message);
            }
        }
    }

    // //@Dependencies:
    // // geometry_fbo
    // // position_tfbo
    // // normal_tfbo
    // // albedo_specular_tfbo
    // // depth_rbo
    void init_opengl(const int frame_buffer_width, const int frame_buffer_height)
    {
        //----init opengl
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            // set clear color to WHITE.
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            // Enable use of the z-buffer
            glEnable(GL_DEPTH_TEST); // default is GL_LESS

            // Line width
            glLineWidth(1.0);
            glEnable(GL_LINE_SMOOTH);

            // enable debug output
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(opengl_message_callback, 0);

            // DISABLE transparency
            glDisable(GL_BLEND);
        }
    
        // query openGL status
        {
            int32_t max_texture_image_units;
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_image_units);
            logr::report("[OpenGL] max combined texture image units: {}\n", max_texture_image_units);

            int32_t max_feedback_buffers;
            glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &max_feedback_buffers);
            logr::report("[OpenGL] max feedback buffers: {}\n", max_feedback_buffers);

            int32_t max_shader_storage_block_size;
            glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_shader_storage_block_size);
            logr::report("[OpenGL] max shader storage block size: {}\n", max_shader_storage_block_size);

            int32_t max_fragment_texture_image_units;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_fragment_texture_image_units);
            logr::report("[OpenGL] max fragment shader texture units: {}\n", max_fragment_texture_image_units);

            int32_t max_uniform_block_size;
            glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
            logr::report("[OpenGL] max uniform block size: {}\n", max_uniform_block_size);

            int32_t max_vertex_uniform_blocks;
            glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &max_vertex_uniform_blocks);
            logr::report("[OpenGL] max Vertex uniform blocks: {}\n", max_vertex_uniform_blocks);

            int32_t max_geometry_uniform_blocks;
            glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &max_geometry_uniform_blocks);
            logr::report("[OpenGL] max Geometry uniform blocks: {}\n", max_geometry_uniform_blocks);

            int32_t max_fragment_uniform_blocks;
            glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &max_fragment_uniform_blocks);
            logr::report("[OpenGL] max Fragment uniform blocks: {} \n", max_fragment_uniform_blocks);

            int32_t max_frame_buffer_color_attachments;
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_frame_buffer_color_attachments);
            logr::report("[OpenGL] max fragment uniform blocks: {}\n", max_fragment_uniform_blocks);            
        }      

        // init Deferred renderer context (position, normal, diffuse + specular, roughness, metallic, ambient_occlusion)
        // order:
        // position,
        // normal
        // diffuse / specular,
        // roughness
        // metallic
        // ambient_occlusion
        // displacement
        // -----------------------------------------------------------
        {
            // init geometry_fbo
            glGenFramebuffers(1, &geometry_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);
            
            // register all three framebuffers as color attachment (GL_COLOR_ATTACHMENTN), N -> {0 : position, 1: normals , 2: albedo & specular}
            
            // - position frame buffer
            const uint32_t position_tfbo = register_framebuffer_texture(*texture_manager, "position_tfbo");
            const auto& position_texture = get_texture(*texture_manager, "position_tfbo");

            glActiveTexture(GL_TEXTURE0 + position_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, position_tfbo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position_tfbo, 0);
              
            // - normals frame buffer
            const uint32_t normal_tfbo = register_framebuffer_texture(*texture_manager, "normal_tfbo");
            const auto& normal_texture = get_texture(*texture_manager, "normal_tfbo");
            glActiveTexture(GL_TEXTURE0 + normal_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, normal_tfbo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tfbo, 0);
              
            // - color + specular frame buffer
            uint32_t albedo_specular_tfbo = register_framebuffer_texture(*texture_manager, "albedo_specular_tfbo");
            auto& albedo_specular_texture = get_texture(*texture_manager, "albedo_specular_tfbo");

            glActiveTexture(GL_TEXTURE0 + albedo_specular_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, albedo_specular_tfbo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo_specular_tfbo, 0);
              
            // roughness frame buffer
            uint32_t roughness_tfbo = register_framebuffer_texture(*texture_manager, "roughness_tfbo"); 
            auto& roughness_texture = get_texture(*texture_manager, "roughness_tfbo");

            glActiveTexture(GL_TEXTURE0 + roughness_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, roughness_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, roughness_tfbo, 0);

            // metallic frame buffer.
            uint32_t metallic_tfbo = register_framebuffer_texture(*texture_manager, "metallic_tfbo");
            auto& metallic_texture = get_texture(*texture_manager, "metallic_tfbo");

            glActiveTexture(GL_TEXTURE0 + metallic_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, metallic_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, metallic_tfbo, 0);

            // ambient occlusion frame buffer.
            uint32_t ambient_occlusion_tfbo = register_framebuffer_texture(*texture_manager, "ambient_occlusion_tfbo");
            auto& ambient_occlusion_texture = get_texture(*texture_manager, "ambient_occlusion_tfbo");

            glActiveTexture(GL_TEXTURE0 + ambient_occlusion_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, ambient_occlusion_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, ambient_occlusion_tfbo, 0);

            // displacement frame buffer
            uint32_t displacement_tfbo = register_framebuffer_texture(*texture_manager, "displacement_tfbo");
            auto& displacement_texture = get_texture(*texture_manager, "displacement_tfbo");

            glActiveTexture(GL_TEXTURE0 + displacement_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, displacement_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, displacement_tfbo, 0);

            uint32_t tnormal_tfbo = register_framebuffer_texture(*texture_manager, "tnormal_tfbo");
            auto& tnormal_texture = get_texture(*texture_manager, "tnormal_tfbo");

            glActiveTexture(GL_TEXTURE0 + tnormal_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, tnormal_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, tnormal_tfbo, 0);


            // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
            unsigned int attachments[8] = {
                GL_COLOR_ATTACHMENT0, // fragment position
                GL_COLOR_ATTACHMENT1, // fragment normal
                GL_COLOR_ATTACHMENT2, // albedo_specular
                GL_COLOR_ATTACHMENT3, // roughness
                GL_COLOR_ATTACHMENT4, // metallic
                GL_COLOR_ATTACHMENT5, // ambient_occlusion
                GL_COLOR_ATTACHMENT6, // displacement
                GL_COLOR_ATTACHMENT7}; // texture_normal

            glDrawBuffers(8, attachments);
              
            // then also add render buffer object as depth buffer and check for completeness.
            unsigned int depth_rbo;
            glGenRenderbuffers(1, &depth_rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, frame_buffer_width, frame_buffer_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

            // finally check if framebuffer is complete
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logr::report_error("[OpenGL] deferred Framebuffer is incomplete.\n");

            // unbind geometry framebuffer default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        // init uniform buffers
        {
            // light buffer

            //@Volatile(Sjors): this should match both the size and the binding index of the buffer in ALL shaders.
            const int LIGHT_BUFFER_UNIFORM_IDX = 2;
            const int light_buffer_size = NUM_LIGHTS * sizeof(Light);
            glCreateBuffers(1, &light_ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
            glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BUFFER_UNIFORM_IDX, light_ubo); 
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

        }

        // // Shadow Map
        // //------------------------------------------------------
        // glGenFramebuffers(1, &depth_map_fbo);
        // g_depth_map_tfbo = register_framebuffer_texture(*texture_manager, "depth_map_tfbo");
        // const auto& depth_map_texture = get_texture(*texture_manager, "depth_map_tfbo");
        // glActiveTexture(GL_TEXTURE0 + depth_map_texture.gl_texture_frame);
        // glBindTexture(GL_TEXTURE_2D, g_depth_map_tfbo);
        // glTexImage2D(
        //     GL_TEXTURE_2D, 
        //     0, 
        //     GL_DEPTH_COMPONENT, 
        //     SHADOW_FB_WIDTH, 
        //     SHADOW_FB_HEIGHT, 
        //     0, 
        //     GL_DEPTH_COMPONENT, 
        //     GL_FLOAT, 
        //     NULL);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

        // glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_depth_map_tfbo, 0);
        // glDrawBuffer(GL_NONE);
        // glReadBuffer(GL_NONE);
        // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logr::report_error("[OpenGL] Shadow framebuffer is incomplete.\n");

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    }

    void init_parallax_cube()
    {
        logr::report_warning("Loading cube.obj instead of dodecahedron!\n");
        // const auto& mesh = get_obj(*asset_manager, "sphere");
        const auto& mesh = get_obj(*asset_manager, "cube");


        glGenVertexArrays(1, &g_parallax_cube_vao);
        glGenBuffers(1, &g_parallax_cube_vbo);
        
        glBindVertexArray(g_parallax_cube_vao);

        // fill buffer with vertices.
        glBindBuffer(GL_ARRAY_BUFFER, g_parallax_cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.interleaved_XNTBU.size() * sizeof(float), mesh.interleaved_XNTBU.data(), GL_STATIC_DRAW);

        // init vertex attributes (0: position, 1: normals, 2: binormals, 3:bitangent. 2: texture coordinates);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(9 * sizeof(float)));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(12 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void init_dodecahedron()
    {
        logr::report_warning("Loading cube.obj instead of dodecahedron!\n");
        const auto& mesh = get_obj(*asset_manager, "cube");
        // const auto& mesh = get_obj(*asset_manager, "dodecahedron");


        glGenVertexArrays(1, &g_dodecahedron_vao);
        glGenBuffers(1, &g_dodecahedron_vbo);

        glBindVertexArray(g_dodecahedron_vao);
        
        // fill buffer with vertices.
        glBindBuffer(GL_ARRAY_BUFFER, g_dodecahedron_vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.interleaved_XNU.size() * sizeof(float), mesh.interleaved_XNU.data(), GL_STATIC_DRAW);
        
        // enable vertex attributes (0: position (xyz), 1: normals (xyz), 2: texture coordinates (uv));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        // initialize two buffers for model matrices and mvp matrices for instanced objects.
        glGenBuffers(1, &g_dodecahedron_model_vbo);
        glGenBuffers(1, &g_dodecahedron_mvp_vbo);

        //@IC(Sjors): BIND the correct buffer, since glVertexAttribPointer refers directly to the bound GL_ARRAY_BUFFER.
        glBindBuffer(GL_ARRAY_BUFFER, g_dodecahedron_model_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_dodecahedron_model_matrices.size(), g_dodecahedron_model_matrices.data(), GL_DYNAMIC_DRAW);

        // since location 0,1,2 are occupied by position, normal, texture coords, 
        const uint32_t model_location = 3;
        const uint32_t mat4_row_count = 4;
        for (unsigned int location_offset = 0; location_offset != mat4_row_count; ++location_offset)
        {
            glEnableVertexAttribArray(model_location + location_offset);
            glVertexAttribPointer(model_location + location_offset, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * location_offset * 4));
            glVertexAttribDivisor(model_location + location_offset, 1);
        }
        
        //@IC(Sjors): BIND the correct buffer, since glVertexAttribPointer refers directly to the bound GL_ARRAY_BUFFER.
        glBindBuffer(GL_ARRAY_BUFFER, g_dodecahedron_mvp_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_dodecahedron_mvp_matrices.size(), g_dodecahedron_mvp_matrices.data(), GL_DYNAMIC_DRAW);

        const uint32_t mvp_location = 7;
        for (unsigned int location_offset = 0; location_offset != mat4_row_count; ++location_offset)
        {

            glEnableVertexAttribArray(mvp_location + location_offset);
            glVertexAttribPointer(mvp_location + location_offset, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * location_offset * 4));
            glVertexAttribDivisor(mvp_location + location_offset, 1);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    void init_instanced_wall()
    {
        // front face
        float vertices[] = {
        -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        };

        glGenVertexArrays(1, &g_wall_vao);
        glGenBuffers(1, &g_wall_vbo);

        glBindVertexArray(g_wall_vao);
        
        // fill buffer with vertices.
        glBindBuffer(GL_ARRAY_BUFFER, g_wall_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        // enable vertex attributes (0: position (xyz), 1: normals (xyz), 2: texture coordinates (uv));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        // initialize two buffers for model matrices and mvp matrices for instanced objects.
        glGenBuffers(1, &g_wall_model_vbo);
        glGenBuffers(1, &g_wall_mvp_vbo);

        //@IC(Sjors): BIND the correct buffer, since glVertexAttribPointer refers directly to the bound GL_ARRAY_BUFFER.
        glBindBuffer(GL_ARRAY_BUFFER, g_wall_model_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_wall_model_matrices.size(), g_wall_model_matrices.data(), GL_DYNAMIC_DRAW);

        // since location 0,1,2 are occupied by position, normal, texture coords, 
        // the matrix starts at location 3.
        // @Note(Sjors): since a location occupies at most 16 bytes (vec4),
        // we need to reserve 4 location indices for any particular matrix.
        // this means location 3,4,5,6 all are reserved for the model matrix,
        // and location 7,8,9,10 are reserved for the mvp matrix.
        const uint32_t model_location = 3;
        const uint32_t mat4_row_count = 4;
        for (unsigned int location_offset = 0; location_offset != mat4_row_count; ++location_offset)
        {
            glEnableVertexAttribArray(model_location + location_offset);
            glVertexAttribPointer(model_location + location_offset, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * location_offset * 4));
            glVertexAttribDivisor(model_location + location_offset, 1);
        }
        
        //@IC(Sjors): BIND the correct buffer, since glVertexAttribPointer refers directly to the bound GL_ARRAY_BUFFER.
        glBindBuffer(GL_ARRAY_BUFFER, g_wall_mvp_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * g_wall_mvp_matrices.size(), g_wall_mvp_matrices.data(), GL_DYNAMIC_DRAW);

        const uint32_t mvp_location = 7;
        for (unsigned int location_offset = 0; location_offset != mat4_row_count; ++location_offset)
        {

            glEnableVertexAttribArray(mvp_location + location_offset);
            glVertexAttribPointer(mvp_location + location_offset, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * location_offset * 4));
            glVertexAttribDivisor(mvp_location + location_offset, 1);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    void init_viewmodel()
    {
        glGenVertexArrays(1, &g_viewmodel_vao);
        glGenBuffers(1, &g_viewmodel_vbo);
        glBindVertexArray(g_viewmodel_vao);

        auto &viewmodel = get_obj(*asset_manager,"new_spear");
        g_viewmodel_vertex_count = viewmodel.interleaved_XNU.size() / 8;

         // fill buffer with vertices.
        glBindBuffer(GL_ARRAY_BUFFER, g_viewmodel_vbo);
        glBufferData(GL_ARRAY_BUFFER, viewmodel.interleaved_XNU.size() * sizeof(float), viewmodel.interleaved_XNU.data(), GL_STATIC_DRAW);
        
        // enable vertex attributes (0: position (xyz), 1: normals (xyz), 2: texture coordinates (uv));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    //@NOTE: NDC is -1,-1 to 1,1. middle is thus 0.0
    void init_hud()
    {
        const float min_x = -0.009f;
        const float max_x = 0.009f;
        const float min_y = -0.016f;
        const float max_y = 0.016f;
        // 16:9
        float vertices[] = {
            // positions        // texture Coords
            min_x, max_y, 0.0f, 0.0f, 1.0f,
            min_x, min_y, 0.0f, 0.0f, 0.0f,
            max_x, max_y, 0.0f, 1.0f, 1.0f,
            max_x, min_y, 0.0f, 1.0f, 0.0f,
        };
        // setup screen plane VAO
        glGenVertexArrays(1, &g_hud_vao);
        glGenBuffers(1, &g_hud_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_hud_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glBindVertexArray(g_hud_vao);

        // init vertex attributes. {0: position (xyz), 1: texture coords (uv)}
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    void init_unit_cube()
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &g_cube_vao);
        glGenBuffers(1, &g_cube_vbo);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glBindVertexArray(g_cube_vao);

        // init vertex attributes (0: position, 1: normals, 2: texture coordinates);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void init_NDC_quad()
    {
        float quad_vertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup screen plane VAO
        glGenVertexArrays(1, &g_quad_vao);
        glGenBuffers(1, &g_quad_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
        glBindVertexArray(g_quad_vao);

        // init vertex attributes. {0: position (xyz), 1: texture coords (uv)}
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    void init_floor()
    {
        const float floor_dim = 2048.0f; // centered, so from -1024 to +1024
        const float half_floor_dim = 2048.0f / 2.0f;
        const float floor_y = -10.0f;
        const float wrap_count = 2048.0f / 64.0f;
        // const float  wrap_count = 1.0f;
      
        float floor_vertices[]  = {
            // |position                               | normals         | uv
            -half_floor_dim, floor_y, half_floor_dim,  0.0f, 1.0f, 0.0f, 0.0f, wrap_count * 1.0f, 
             half_floor_dim, floor_y, half_floor_dim,  0.0f, 1.0f, 0.0f, wrap_count * 1.0f, wrap_count * 1.0f,
            -half_floor_dim, floor_y, -half_floor_dim, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -half_floor_dim, floor_y, -half_floor_dim, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
             half_floor_dim, floor_y, half_floor_dim,  0.0f, 1.0f, 0.0f, wrap_count * 1.0f, wrap_count * 1.0f,
             half_floor_dim, floor_y, -half_floor_dim, 0.0f, 1.0f, 0.0f, wrap_count * 1.0f, 0.0f
        };
        glGenVertexArrays(1, &g_floor_vao);
        glBindVertexArray(g_floor_vao);
        glGenBuffers(1, &g_floor_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_floor_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), &floor_vertices, GL_STATIC_DRAW);

        // link vertex attributes (position, normals, texture coordinates);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }

    void init_debug_geometry()
    {
        glGenVertexArrays(1, &g_debug_geometry_vao);
        glBindVertexArray(g_debug_geometry_vao);

        glGenBuffers(1, &g_debug_geometry_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_debug_geometry_vbo);

         // link vertex attributes (position, normals, color);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));


        // add initial buffer to data.
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) *g_debug_draw_data.size(), g_debug_draw_data.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

}

void init_renderer(
    Shader_Manager& shader_manager_in, 
    Texture_Manager& texture_manager_in, 
    Asset_Manager& asset_manager_in,
    Entity_Manager& entity_manager_in,

    const int frame_buffer_width, const int frame_buffer_height)
{
    // initialize globals.
    shader_manager  = &shader_manager_in;
    texture_manager = &texture_manager_in;
    asset_manager   = &asset_manager_in;
    entity_manager  = &entity_manager_in;

    g_window_width  = frame_buffer_width;
    g_window_height = frame_buffer_height;

    g_aspect_ratio = static_cast<float>(g_window_width) / static_cast<float>(g_window_height);

    init_opengl(frame_buffer_width, frame_buffer_height);
    init_unit_cube();
    init_NDC_quad();
    init_floor();
    init_instanced_wall();
    init_debug_geometry();
    init_viewmodel();
    init_hud();
    init_dodecahedron();
    init_parallax_cube();
}


void render_parallax_cube()
{
    // render Cube
    glBindVertexArray(g_parallax_cube_vao);

    // size_t float_count = 36;
    // auto& obj = get_obj(*asset_manager, "sphere");
    auto& obj = get_obj(*asset_manager, "cube");

    size_t float_count = obj.interleaved_XNTBU.size();

    glDrawArrays(GL_TRIANGLES, 0, float_count);
    glBindVertexArray(0);
}



void render_cube()
{
    // render Cube
    glBindVertexArray(g_cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// render_NDC_quad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void render_NDC_quad()
{  
    glBindVertexArray(g_quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void render_floor()
{
    glBindVertexArray(g_floor_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void render_viewmodel()
{
    glBindVertexArray(g_viewmodel_vao);
    glDrawArrays(GL_TRIANGLES, 0, g_viewmodel_vertex_count);
    glBindVertexArray(0);
}

// render_NDC_quad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void render_hud()
{  
    glBindVertexArray(g_hud_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


// @dependencies:
// shader_manager, texture_manager
// camera
/// - particles
// cvars: g_fov, g_aspect_ratio, g_projection_z_near_plane, g_projection_z_far_plane
/// - lights
/// - the geometry that needs to be rendered (either via submission or other)
void render(const Camera camera, Particle_Cache& particle_cache)
{
    timed_function("render");
    // render
    // ------
    Camera player_camera = camera;
    glm::mat4 view       = create_view_matrix_from_camera(player_camera);
    glm::mat4 projection = glm::perspective(glm::radians(g_fov), g_aspect_ratio, g_projection_z_near_plane, g_projection_z_far_plane);  

    // 1. geometry pass: render scene's geometry/color data into geometry_fbo
    // -----------------------------------------------------------------
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind the geometry buffer as the frame buffer.
        glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 0? PBR
        // ------------------------------------------
   
        {
            set_shader(*shader_manager, "deferred_pbr");
            set_uniform(*shader_manager, "projection", projection);
            set_uniform(*shader_manager, "view", view);

            const auto& moss_albedo_texture = get_texture(*texture_manager, "moss_2K_color");
            const auto& moss_normal_texture = get_texture(*texture_manager, "moss_2K_normal");
            const auto& moss_roughness_texture = get_texture(*texture_manager, "moss_2K_roughness");
            // const auto& moss_metallic_texture does not exist :^)
            const auto& moss_ambient_occlusion_texture = get_texture(*texture_manager, "moss_2K_ambient_occlusion");
            const auto& moss_displacement_texture = get_texture(*texture_manager, "moss_2K_displacement");

            set_uniform(*shader_manager, "texture_albedo", moss_albedo_texture.gl_texture_frame);
            set_uniform(*shader_manager, "texture_normal", moss_normal_texture.gl_texture_frame);
            set_uniform(*shader_manager, "texture_roughness", moss_roughness_texture.gl_texture_frame);
            set_uniform(*shader_manager, "texture_metallic", moss_roughness_texture.gl_texture_frame); // this does not exist.
            set_uniform(*shader_manager, "texture_ambient_occlusion", moss_ambient_occlusion_texture.gl_texture_frame);
            set_uniform(*shader_manager, "texture_displacement", moss_displacement_texture.gl_texture_frame);

            // // render floor
            glm::mat4 model = glm::mat4(1.0f);
            set_uniform(*shader_manager, "model", model);
            render_floor();
             
            // render walls
            {
                timed_function("render_walls");

                set_shader(*shader_manager, "deferred_instanced_pbr");
                // set_uniform(*shader_manager, "view", view); // this does not exist. only mvp.
                // set_uniform(*shader_manager, "projection", projection); this does not exist. only mvp.

                const auto& painted_metal_albedo_texture = get_texture(*texture_manager, "painted_metal_2K_color");
                const auto& painted_metal_normal_texture = get_texture(*texture_manager, "painted_metal_2K_normal");
                const auto& painted_metal_roughness_texture = get_texture(*texture_manager, "painted_metal_2K_roughness");
                const auto& painted_metal_metallic_texture = get_texture(*texture_manager, "painted_metal_2K_metallic");
                const auto& painted_metal_ambient_occlusion_texture = get_texture(*texture_manager, "painted_metal_2K_ambient_occlusion");
                const auto& painted_metal_displacement_texture = get_texture(*texture_manager, "painted_metal_2K_displacement");

                set_uniform(*shader_manager, "texture_albedo", painted_metal_albedo_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_normal", painted_metal_normal_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_roughness", painted_metal_roughness_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_metallic", painted_metal_metallic_texture.gl_texture_frame); // this does not exist.
                set_uniform(*shader_manager, "texture_ambient_occlusion", painted_metal_ambient_occlusion_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_displacement", painted_metal_displacement_texture.gl_texture_frame);

                // update matrix of the walls.
                {
                    auto& model_matrix_0 = g_wall_model_matrices[0];
                    auto& mvp_matrix_0 = g_wall_mvp_matrices[0];
                    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
                    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(1024.0f, 0.0f, 0.0f));
                    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), -0.5f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
                    model_matrix_0 = translate * rotate * scale;
                    mvp_matrix_0 = projection * view * model_matrix_0;  

                    auto& model_matrix_1 = g_wall_model_matrices[1];
                    auto& mvp_matrix_1 = g_wall_mvp_matrices[1];
                    scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
                    translate = glm::translate(glm::mat4(1.0f), glm::vec3(-1024.0f, 0.0f, 0.0f));
                    rotate = glm::rotate(glm::mat4(1.0f), 0.5f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
                    model_matrix_1 = translate * rotate * scale;
                    mvp_matrix_1 = projection * view * model_matrix_1;  

                    auto& model_matrix_2 = g_wall_model_matrices[2];
                    auto& mvp_matrix_2 = g_wall_mvp_matrices[2];
                    scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
                    translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1024.0f));
                    rotate = glm::rotate(glm::mat4(1.0f), 0.0f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
                    model_matrix_2 = translate * rotate * scale;
                    mvp_matrix_2 = projection * view * model_matrix_2;  

                    auto& model_matrix_3 = g_wall_model_matrices[3];
                    auto& mvp_matrix_3 = g_wall_mvp_matrices[3];
                    scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
                    translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1024.0f));
                    rotate = glm::rotate(glm::mat4(1.0f), -1.0f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
                    model_matrix_3 = translate * rotate * scale;
                    mvp_matrix_3 = projection * view * model_matrix_3;  
                }

                glNamedBufferData(g_wall_model_vbo, sizeof(glm::mat4) * g_wall_model_matrices.size(), g_wall_model_matrices.data(), GL_DYNAMIC_DRAW);
                glNamedBufferData(g_wall_mvp_vbo,   sizeof(glm::mat4) * g_wall_mvp_matrices.size(),   g_wall_mvp_matrices.data(), GL_DYNAMIC_DRAW);


                glBindVertexArray(g_wall_vao);
                glDrawArraysInstanced(
                    GL_TRIANGLES,
                    0,
                    6,
                    g_wall_model_matrices.size());
                glBindVertexArray(0);
            }
        
            // // render spheres
            // {
            //     timed_function("render_spheres");

            //     set_shader(*shader_manager, "deferred_instanced_pbr");
            //     // set_uniform(*shader_manager, "view", view);
            //     // set_uniform(*shader_manager, "projection", projection);

            //     const auto& pavingstones_albedo_texture = get_texture(*texture_manager, "pavingstones_4K_color");
            //     const auto& pavingstones_normal_texture = get_texture(*texture_manager, "pavingstones_4K_normal");
            //     const auto& pavingstones_roughness_texture = get_texture(*texture_manager, "pavingstones_4K_roughness");
            //     // const auto& pavingstones_metallic_texture does not exist :^)
            //     const auto& pavingstones_ambient_occlusion_texture = get_texture(*texture_manager, "pavingstones_4K_ambient_occlusion");
            //     const auto& pavingstones_displacement_texture = get_texture(*texture_manager, "pavingstones_4K_displacement");

            //     set_uniform(*shader_manager, "texture_albedo", pavingstones_albedo_texture.gl_texture_frame);
            //     set_uniform(*shader_manager, "texture_normal", pavingstones_normal_texture.gl_texture_frame);
            //     set_uniform(*shader_manager, "texture_roughness", pavingstones_roughness_texture.gl_texture_frame);
            //     // set_uniform(*shader_manager, "texture_metallic", pavingstones_roughness_texture.gl_texture_frame); // this does not exist.
            //     set_uniform(*shader_manager, "texture_ambient_occlusion", pavingstones_ambient_occlusion_texture.gl_texture_frame);
            //     set_uniform(*shader_manager, "texture_displacement", pavingstones_displacement_texture.gl_texture_frame);


            //     size_t draw_count = 0;
            //     // this is not actually part of rendering: we are just updating the buffers.
            //     {
    
            //         glm::mat4& model_matrix_0 = g_dodecahedron_model_matrices[0];
            //         glm::mat4& mvp_matrix_0 = g_dodecahedron_mvp_matrices[0];
            //         glm::mat4& model_matrix_1 = g_dodecahedron_model_matrices[1];
            //         glm::mat4& mvp_matrix_1 = g_dodecahedron_mvp_matrices[1];
            //         glm::mat4& model_matrix_2 = g_dodecahedron_model_matrices[2];
            //         glm::mat4& mvp_matrix_2 = g_dodecahedron_mvp_matrices[2];
            //         glm::mat4& model_matrix_3 = g_dodecahedron_model_matrices[2];
            //         glm::mat4& mvp_matrix_3 = g_dodecahedron_mvp_matrices[2];

            //         glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(20.f));
            //         glm::mat4 translate_0 = glm::translate(glm::mat4(1.f), glm::vec3(300.f, -5.f, 0.f));
            //         glm::mat4 translate_1 = glm::translate(glm::mat4(1.f), glm::vec3(300.f, -5.f, 300.f));
            //         glm::mat4 translate_2 = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -5.f, 300.f));
            //         glm::mat4 translate_3 = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -5.f, 0.0f));
            //         glm::mat4 rotate = glm::mat4(1.0f);

            //         model_matrix_0 = translate_0 * rotate * scale;
            //         model_matrix_1 = translate_1 * rotate * scale;
            //         model_matrix_2 = translate_2 * rotate * scale;
            //         model_matrix_3 = translate_3 * rotate * scale;
            //         mvp_matrix_0 = projection * view * model_matrix_0;
            //         mvp_matrix_1 = projection * view * model_matrix_1;
            //         mvp_matrix_2 = projection * view * model_matrix_2;
            //         mvp_matrix_3 = projection * view * model_matrix_3;

            //         draw_count = 4;


            //         glNamedBufferData(g_dodecahedron_model_vbo, sizeof(glm::mat4) * g_dodecahedron_model_matrices.size(), g_dodecahedron_model_matrices.data(), GL_DYNAMIC_DRAW);
            //         glNamedBufferData(g_dodecahedron_mvp_vbo,   sizeof(glm::mat4) * g_dodecahedron_mvp_matrices.size(),   g_dodecahedron_mvp_matrices.data(), GL_DYNAMIC_DRAW);
            //     }
                
            //     const size_t mesh_size = get_obj(*asset_manager, "cube").interleaved_XNU.size() / 8;

            //     glBindVertexArray(g_dodecahedron_vao);
            //     glDrawArraysInstanced(
            //         GL_TRIANGLES,
            //         0,
            //         mesh_size,
            //         draw_count);


            //     glBindVertexArray(0);
            // }


        }



        // // bind shader and update all relevant uniforms.
        // set_shader(*shader_manager, "deferred_geometry");
        // set_uniform(*shader_manager, "projection", projection);

        // // 1.a:  render static geometry in the scene.
        // // ---------------------------------------
        // {
        //     // render weapon(viewmodel). These values are arbitrary as of now.
        //     {
        //         const auto& spear_texture = get_texture(*texture_manager, "metal");
        //         set_uniform(*shader_manager,  "texture_diffuse", spear_texture.gl_texture_frame);
        //         set_uniform(*shader_manager, "view", glm::mat4(1.0f));

        //         glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
        //         glm::quat quaternion = glm::quat(0.70f, -0.70f, 0.0f, 0.0f);
        //         glm::mat4 rotate = glm::toMat4(quaternion);
        //         glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f,-1.0f, -0.7f));
        //         glm::mat4 model = translate * rotate * scale;

        //         set_uniform(*shader_manager, "model", model);
        //         render_viewmodel();
        //     }

        //     set_uniform(*shader_manager, "view", view);
        //     // for all cubes in the scene..)
        //     {
        //         const auto& metal_texture = get_texture(*texture_manager, "metal");
        //         set_uniform(*shader_manager,  "texture_diffuse", metal_texture.gl_texture_frame);

        //         glm::mat4 model = glm::mat4(1.0f);
        //         set_uniform(*shader_manager, "model", model);
        //         render_cube();

        //     }
        
        //     //   render floor
        //     {
        //         const auto& floor_texture = get_texture(*texture_manager, "floor_64");
        //         set_uniform(*shader_manager, "texture_diffuse",  floor_texture.gl_texture_frame);

        //         glm::mat4 model = glm::mat4(1.0f);
        //         set_uniform(*shader_manager, "model", model);
        //         render_floor();
        //     }

        //     // render walls
        //     {
        //         timed_function("render_walls");

        //         set_shader(*shader_manager, "deferred_instanced");
        //         set_uniform(*shader_manager, "view", view);

        //         // update matrix of the walls.
        //         {
        //             auto& model_matrix_0 = g_wall_model_matrices[0];
        //             auto& mvp_matrix_0 = g_wall_mvp_matrices[0];
        //             glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
        //             glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(1024.0f, 0.0f, 0.0f));
        //             glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), -0.5f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
        //             model_matrix_0 = translate * rotate * scale;
        //             mvp_matrix_0 = projection * view * model_matrix_0;  

        //             auto& model_matrix_1 = g_wall_model_matrices[1];
        //             auto& mvp_matrix_1 = g_wall_mvp_matrices[1];
        //             scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
        //             translate = glm::translate(glm::mat4(1.0f), glm::vec3(-1024.0f, 0.0f, 0.0f));
        //             rotate = glm::rotate(glm::mat4(1.0f), 0.5f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
        //             model_matrix_1 = translate * rotate * scale;
        //             mvp_matrix_1 = projection * view * model_matrix_1;  

        //             auto& model_matrix_2 = g_wall_model_matrices[2];
        //             auto& mvp_matrix_2 = g_wall_mvp_matrices[2];
        //             scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
        //             translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1024.0f));
        //             rotate = glm::rotate(glm::mat4(1.0f), 0.0f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
        //             model_matrix_2 = translate * rotate * scale;
        //             mvp_matrix_2 = projection * view * model_matrix_2;  

        //             auto& model_matrix_3 = g_wall_model_matrices[3];
        //             auto& mvp_matrix_3 = g_wall_mvp_matrices[3];
        //             scale = glm::scale(glm::mat4(1.0f), glm::vec3(1024.0f));
        //             translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1024.0f));
        //             rotate = glm::rotate(glm::mat4(1.0f), -1.0f * M_PI, glm::vec3(0.0f,1.0f,0.0f));
        //             model_matrix_3 = translate * rotate * scale;
        //             mvp_matrix_3 = projection * view * model_matrix_3;  
        //         }

        //         glNamedBufferData(g_wall_model_vbo, sizeof(glm::mat4) * g_wall_model_matrices.size(), g_wall_model_matrices.data(), GL_DYNAMIC_DRAW);
        //         glNamedBufferData(g_wall_mvp_vbo,   sizeof(glm::mat4) * g_wall_mvp_matrices.size(),   g_wall_mvp_matrices.data(), GL_DYNAMIC_DRAW);

        //         const auto& wall_stone_diffuse_texture  = get_texture(*texture_manager, "wall_stone_diffuse");
        //         const auto& wall_stone_specular_texture = get_texture(*texture_manager, "wall_stone_specular");
        //         const auto& wall_stone_normal_texture   = get_texture(*texture_manager, "wall_stone_normal");

        //         set_uniform(*shader_manager, "texture_diffuse",  wall_stone_diffuse_texture.gl_texture_frame);
        //         set_uniform(*shader_manager, "texture_specular", wall_stone_specular_texture.gl_texture_frame);

        //         glBindVertexArray(g_wall_vao);
        //         glDrawArraysInstanced(
        //             GL_TRIANGLES,
        //             0,
        //             6,
        //             g_wall_model_matrices.size());
        //         glBindVertexArray(0);
        //     }

        //     // render dodecahedrons
        //     {
        //         timed_function("render_dodecahedron");

        //         set_shader(*shader_manager, "deferred_instanced");
        //         set_uniform(*shader_manager, "view", view);
                

        //         size_t draw_count = 0;
        //         // this is not actually part of rendering: we are just updating the buffers.
        //         {
               
        //             //@Note(Sjors): we now use draw count in order to not overdraw.
        //             // draw_count is used in gldrawarraysinstanced.
        //             // however, this means that there is garbage at the end of the buffer (past draw count).
        //             // we may want to zero that.
        //             Entity_Manager& manager = *entity_manager;
        //             for (auto&& entity: by_type(manager, Entity_Type::Cube))
        //             {

        //                 if (entity.scheduled_for_destruction) continue;

        //                 glm::mat4& model_matrix = g_dodecahedron_model_matrices[draw_count];
        //                 glm::mat4& mvp_matrix = g_dodecahedron_mvp_matrices[draw_count];
        //                 draw_count += 1;

        //                 glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(20.f));
                        
        //                 // turn angry face towards player
        //                 glm::mat4 rotate = glm::mat4(1.0f);
        //                 glm::vec3 target_ray = glm::normalize(camera.position - entity.position);
        //                 glm::vec3 object_ray = glm::vec3(0.f,1.f, 0.f);
        //                 float angle_dif = acos(glm::dot(target_ray, object_ray));
        //                 if (angle_dif != 0)
        //                 {
        //                     glm::vec3 ortho_ray = glm::cross(object_ray, target_ray);
        //                     ortho_ray = glm::normalize(ortho_ray);
        //                     glm::quat delta_quaternion= glm::angleAxis(angle_dif, ortho_ray);
        //                     rotate = glm::toMat4(delta_quaternion);
        //                 }

        //                 glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);
        //                 model_matrix = translate * rotate * scale;
        //                 mvp_matrix = projection * view * model_matrix;  
        //             }

        //             glNamedBufferData(g_dodecahedron_model_vbo, sizeof(glm::mat4) * g_dodecahedron_model_matrices.size(), g_dodecahedron_model_matrices.data(), GL_DYNAMIC_DRAW);
        //             glNamedBufferData(g_dodecahedron_mvp_vbo,   sizeof(glm::mat4) * g_dodecahedron_mvp_matrices.size(),   g_dodecahedron_mvp_matrices.data(), GL_DYNAMIC_DRAW);
        //         }
            
        //         const auto& dodecahedron_diffuse_texture  = get_texture(*texture_manager, "angry_face");
        //         const auto& wall_stone_specular_texture = get_texture(*texture_manager, "wall_stone_specular");

        //         set_uniform(*shader_manager, "texture_diffuse",  dodecahedron_diffuse_texture.gl_texture_frame);
        //         set_uniform(*shader_manager, "texture_specular", wall_stone_specular_texture.gl_texture_frame);
                
        //         const size_t mesh_size = get_obj(*asset_manager, "dodecahedron").interleaved_XNU.size() / 8;

        //         glBindVertexArray(g_dodecahedron_vao);
        //         glDrawArraysInstanced(
        //             GL_TRIANGLES,
        //             0,
        //             mesh_size,
        //             draw_count);


        //         glBindVertexArray(0);
        //     }

        // }
       
        // unbind geometry buffer, bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    // @Volatile(Sjors): if this changes, the deferred_lighting shader step should change as well.
    std::vector<Light> lights(NUM_LIGHTS);

    //  2.post deferred PBR
    { 
        const auto& position_tfbo_texture = get_texture(*texture_manager, "position_tfbo");
        const auto& normal_tfbo_texture = get_texture(*texture_manager, "normal_tfbo");
        const auto& albedo_specular_tfbo_texture = get_texture(*texture_manager, "albedo_specular_tfbo");
        const auto& roughness_tfbo_texture= get_texture(*texture_manager, "roughness_tfbo");
        const auto& metallic_tfbo_texture = get_texture(*texture_manager, "metallic_tfbo");
        const auto& ambient_occlusion_tfbo_texture = get_texture(*texture_manager, "ambient_occlusion_tfbo");
        const auto& displacement_tfbo_texture = get_texture(*texture_manager, "displacement_tfbo");
        const auto& tnormal_tfbo_texture = get_texture(*texture_manager, "tnormal_tfbo");

        set_shader(*shader_manager, "post_deferred_pbr");

        set_uniform(*shader_manager,"fb_position", position_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager,"fb_normal", normal_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager,"fb_albedo_spec", albedo_specular_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager,"fb_roughness", roughness_tfbo_texture.gl_texture_frame);
        // set_uniform(*shader_manager,"fb_metallic", metallic_tfbo_texture.gl_texture_frame); // disabled
        set_uniform(*shader_manager,"fb_ambient_occlusion", ambient_occlusion_tfbo_texture.gl_texture_frame);
        // set_uniform(*shader_manager,"fb_displacement", displacement_tfbo_texture.gl_texture_frame); // disabled
        set_uniform(*shader_manager,"fb_tnormal", tnormal_tfbo_texture.gl_texture_frame);

        glm::vec4 camera_position = glm::vec4(camera.position, 1.0f); 
        set_uniform(*shader_manager, "view_position", glm::vec4(camera_position));

        { 
            static float time = 0.0f;
            static float y_position = 0.0f;
            static float x_position = 0.0f;
            time += 2.0f;
            time = fmod(time, 6280.0f);
            y_position = sin(time/ 1000.0f);
            x_position = cos(time/ 1000.0f);

            // player light.
            lights[0].position.x = camera.position.x;
            lights[0].position.y = camera.position.y;
            lights[0].position.z = camera.position.z;
            lights[0].color = glm::vec4(1.0f,1.0f,1.0f,0.0f);
            lights[0].on = true;
            lights[0].linear = 0.1f;
            lights[0].quadratic = 0.2f;

            // lights[1].position = glm::vec4(0.0f, 3.0f, 0.0f, 0.0f);
            // lights[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
            // lights[1].on = true;
            // lights[1].linear = 0.1;
            // lights[1].quadratic = 0.2;
        }
        //@Note(Sjors): while not necessary to bind the buffer
        // when using glnamedbufferdata to update the buffer. it IS important to still bind it before rendering the
        // NDC quad, since any glDraw* call will take the currently bound array / index buffer.
        glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
        glNamedBufferData(light_ubo, lights.size() * sizeof(Light),  lights.data(), GL_DYNAMIC_DRAW);

        render_NDC_quad();
    }


    // // 2. lighting pass:
    // // -----------------------------------------------------------------
    // {
    //     // get texture handles for the frame buffers.
    //     const auto& position_tfbo_texture = get_texture(*texture_manager, "position_tfbo");
    //     const auto& normal_tfbo_texture = get_texture(*texture_manager, "normal_tfbo");
    //     const auto& albedo_specular_tfbo_texture = get_texture(*texture_manager, "albedo_specular_tfbo");

    //     set_shader(*shader_manager, "post_deferred_lighting");

    //     set_uniform(*shader_manager, "fb_position",    position_tfbo_texture.gl_texture_frame);
    //     set_uniform(*shader_manager, "fb_normal",      normal_tfbo_texture.gl_texture_frame);    
    //     set_uniform(*shader_manager, "fb_albedo_spec", albedo_specular_tfbo_texture.gl_texture_frame);    

    //     glm::vec4 camera_position = glm::vec4(camera.position, 1.0f); 
    //     set_uniform(*shader_manager, "view_position", glm::vec4(camera_position));

    //     // for (auto& light: lights)
    //     {
    //         static float time = 0.0f;
    //         static float y_position = 0.0f;
    //         static float x_position = 0.0f;
    //         time += 2.0f;
    //         time = fmod(time, 6280.0f);
    //         y_position = sin(time/ 1000.0f);
    //         x_position = cos(time/ 1000.0f);

    //         // player light.
    //         lights[0].position.x = camera.position.x;
    //         lights[0].position.y = camera.position.y;
    //         lights[0].position.z = camera.position.z;
    //         lights[0].color = glm::vec4(1.0f,1.0f,1.0f,0.0f);
    //         lights[0].on = true;
    //         lights[0].linear = 0.001f;
    //         lights[0].quadratic = 0.002f;

    //         lights[1].position = glm::vec4(0.0f, 3.0f, 0.0f, 0.0f);
    //         lights[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    //         lights[1].on = true;
    //         lights[1].linear = 0.1;
    //         lights[1].quadratic = 0.2;

    //     }
        
    //     //@Note(Sjors): while not necessary to bind the buffer
    //     // when using glnamedbufferdata to update the buffer. it IS important to still bind it before rendering the
    //     // NDC quad, since any glDraw* call will take the currently bound array / index buffer.
    //     glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
    //     glNamedBufferData(light_ubo, lights.size() * sizeof(Light),  lights.data(), GL_DYNAMIC_DRAW);

    //     render_NDC_quad();
    // }

    // // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    // // ----------------------------------------------------------------------------------
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, geometry_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the      
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).

        glBlitFramebuffer(0, 0, g_window_width, g_window_height, 0, 0, g_window_width, g_window_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    // // // 3. render geometry on top of scene that does not need to be affected by lighting.
    // // --------------------------------
    // {
    //     // only render the active lights.normally, for light: active_lights)
    //     {
    //         set_shader( *shader_manager, "lightbox");
    //         set_uniform(*shader_manager, "projection", projection);
    //         set_uniform(*shader_manager, "view", view);

    //         glm::mat4 model = glm::mat4(1.0f);
    //         model = glm::translate(model, glm::vec3(lights[0].position));
    //         model = glm::scale(model, glm::vec3(0.01f));
    //         set_uniform(*shader_manager, "model", model);
    //         set_uniform(*shader_manager, "light_color", lights[0].color);
    //         render_cube();

    //         model = glm::mat4(1.0f);
    //         model = glm::translate(model, glm::vec3(lights[1].position));
    //         model = glm::scale(model, glm::vec3(0.125f));
    //         set_uniform(*shader_manager, "model", model);
    //         set_uniform(*shader_manager, "light_color", lights[1].color);
    //         render_cube();
    //     }

    //     // render debug data
    //     {
    //         set_shader(*shader_manager, "debug_geometry");

    //         set_uniform(*shader_manager, "view", view);
    //         set_uniform(*shader_manager, "projection", projection);
    //         set_uniform(*shader_manager, "model", glm::mat4(1.0f));

    //         glBindVertexArray(g_debug_geometry_vao);
    //         glBindBuffer(GL_ARRAY_BUFFER, g_debug_geometry_vbo);
    //         glBufferData(GL_ARRAY_BUFFER, sizeof(float) *g_debug_draw_data.size(), g_debug_draw_data.data(), GL_STATIC_DRAW);

    //         // debug draw data = position(3) + normal 3+ color(3)
    //         glDrawArrays(GL_TRIANGLES, 0, g_debug_draw_data.size() / 9);
    //     }
    // }

    // step 4: render HUD
    {   

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
        const auto& crosshair_texture =  get_texture(*texture_manager, "crosshair");
        set_shader(*shader_manager, "screen_space");
        set_uniform(*shader_manager, "hud_texture", crosshair_texture.gl_texture_frame);

        render_hud();

        glDisable(GL_BLEND);
    }


    set_shader(*shader_manager, "none");
    glBindVertexArray(0);
}


void render_shadows(const Camera camera, Particle_Cache& particle_cache)
{  

    // render
    // ------
    Camera player_camera = camera;
    glm::mat4 view       = create_view_matrix_from_camera(player_camera);
    glm::mat4 projection = glm::perspective(glm::radians(g_fov), g_aspect_ratio, g_projection_z_near_plane, g_projection_z_far_plane); 

        // STEP 0: SHADOW MAPPING
    {
        // 0.1: first render to depth map
        // ------------------------------------------
        {
            set_shader(*shader_manager, "simple_depth");
            glViewport(0, 0, SHADOW_FB_WIDTH, SHADOW_FB_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            glClear(GL_DEPTH_BUFFER_BIT);

            const float near_plane = g_projection_z_near_plane;
            const float far_plane = g_projection_z_far_plane;

            static float time = 0.0f;
            static float z_position = 0.0f;
            static float x_position = 0.0f;
            time += 2.0f;
            time = fmod(time, 6280.0f);
            z_position = sin(time/ 1000.0f);
            x_position = cos(time/ 1000.0f);

            // frustum?
            glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            glm::mat4 light_view =       glm::lookAt(glm::vec3(x_position, 3.0f, z_position), // eye
                                                     glm::vec3(0.0f, 0.0f,  0.0f),  // target
                                                     glm::vec3(0.0f, 1.0f,  0.0f));  // up

            glm::mat4 light_space_matrix = light_projection * light_view;

            set_uniform(*shader_manager, "light_space_matrix", light_space_matrix);
            // for all cubes in the scene..
            {
                glm::mat4 model  = glm::mat4(1.0f);
                set_uniform(*shader_manager, "model", model);
                render_cube();
            }
            // unbind framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 0.2. then render scene as normal with shadow mapping (using depth map)
            glViewport(0, 0, g_window_width, g_window_height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            set_shader(*shader_manager, "simple_depth_shadow_mapping");

            set_uniform(*shader_manager, "projection", projection);
            set_uniform(*shader_manager, "view", view);
            // set light uniforms
            set_uniform(*shader_manager, "light_pos", glm::vec3(x_position, 3.0f, z_position));
            set_uniform(*shader_manager, "view_pos", glm::vec3(camera.position.x, camera.position.y, camera.position.z));
            set_uniform(*shader_manager, "light_space_matrix", light_space_matrix);

            const auto& diffuse_texture = get_texture(*texture_manager, "metal");
            const auto& shadow_map = get_texture(*texture_manager, "depth_map_tfbo");
            set_uniform(*shader_manager, "diffuse_texture", diffuse_texture.gl_texture_frame);
            set_uniform(*shader_manager, "shadow_map", shadow_map.gl_texture_frame);
            
            // for all cubes in the scene..
            {
                glm::mat4 model  = glm::mat4(1.0f);
                set_uniform(*shader_manager, "model", model);
                render_cube();
                render_floor();
            }
            set_shader(*shader_manager, "lightbox");
            set_uniform(*shader_manager, "view", view);
            set_uniform(*shader_manager, "projection", projection);
            // for all cubes in the scene..
            {
                glm::mat4 model  = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x_position, 3.0f, z_position));
                model = glm::scale(model, glm::vec3(0.125f));
                set_uniform(*shader_manager, "model", model);
                set_uniform(*shader_manager, "light_color", glm::vec4(1.0f,1.0f,1.0f,0.0f));

                render_cube();
            }

        }
    }
   
}


//@Globals:
//  NONE_VAO
//  NONE_VBO

// @Assumptions
// GL_STATIC_DRAW for the bufferData (GL_DYNAMIC_DRAW, etc).
// GL_FLOAT for glVertexAttribPointer (the data type / size)
// GL_FALSE for glVertexAttribPointer (normalize fixed-point data values)
// GL_DYNAMIC_DRAW for the bufferData of the model matrices
void create_interleaved_XNU_model_buffer(const std::string& model_name, const std::vector<float>& interleaved_XNU_values, const size_t draw_count)
{
    Model_Buffer model_buffer{};

    glGenVertexArrays(1, &model_buffer.VAO); 
    glGenBuffers(1,      &model_buffer.VBO);
    glBindVertexArray(model_buffer.VAO);

    // fill buffer with vertices.
    glBindBuffer(GL_ARRAY_BUFFER, model_buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, interleaved_XNU_values.size() * sizeof(float), interleaved_XNU_values.data(), GL_STATIC_DRAW);

    const int32_t position_attribute_id = 0;
    const int32_t normal_attribute_id   = 1;
    const int32_t uv_attribute_id       = 2;

    const int32_t position_float_count  = 3;
    const int32_t normal_float_count    = 3;
    const int32_t uv_float_count        = 2;

    const int32_t position_byte_offset  = 0; // 0
    const int32_t normal_byte_offset    =  position_float_count * sizeof(float); // 3
    const int32_t uv_byte_offset        = (position_float_count + normal_float_count) * sizeof(float); // 6

    constexpr int32_t vertex_byte_stride = (position_float_count + normal_float_count + uv_float_count) * sizeof(float); // 8

    // position vertex attribute
    glEnableVertexAttribArray(position_attribute_id);
    glVertexAttribPointer(
        position_attribute_id,
        position_float_count,
        GL_FLOAT,
        GL_FALSE,
        vertex_byte_stride,
        (void*)position_byte_offset
        );

    // normal vertex attribute
    glEnableVertexAttribArray(normal_attribute_id);
    glVertexAttribPointer(
        normal_attribute_id,
        normal_float_count,
        GL_FLOAT,
        GL_FALSE,
        vertex_byte_stride,
        (void*)normal_byte_offset
        );

    // texture vertex attribute
    glEnableVertexAttribArray(uv_attribute_id);
    glVertexAttribPointer(
        uv_attribute_id,
        uv_float_count,
        GL_FLOAT,
        GL_FALSE,
        vertex_byte_stride,
        (void*)uv_byte_offset
        );


    // model matrix buffer.
    {
        //@Memory(Sjors):
        // allocate data for the matrices.
        model_buffer.model_matrix_buffer.resize(draw_count);
        const int buffer_size = draw_count * sizeof(glm::mat4);


        glGenBuffers(1, &model_buffer.model_matrix_VBO);
        //@IC(Sjors): BIND the correct buffer, since glVertexAttribPointer refers directly to the bound GL_ARRAY_BUFFER.
        glBindBuffer(GL_ARRAY_BUFFER, model_buffer.model_matrix_VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            buffer_size,
            model_buffer.model_matrix_buffer.data(),
            GL_DYNAMIC_DRAW
            );

        // since location 0,1,2 are occupied by position, normal, texture coords:
        const int32_t model_matrix_attribute_id = 3;
        const int32_t row_count = 4;
        const int row_float_count = 4;
        const int matrix_byte_stride = sizeof(glm::mat4);
        const int row_byte_stride = sizeof(glm::vec4);

        for (int32_t location_offset = 0; location_offset != row_count; ++location_offset)
        {
            const int row_attribute_id = model_matrix_attribute_id + location_offset;
            glEnableVertexAttribArray(row_attribute_id);
            glVertexAttribPointer(
                row_attribute_id,
                row_float_count,
                GL_FLOAT,
                GL_FALSE,
                matrix_byte_stride,
                (void*)(location_offset * row_byte_stride)
                );
            glVertexAttribDivisor(row_attribute_id, 1);
        } 
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, NONE_VBO);
    glBindVertexArray(NONE_VAO);

    f_model_buffers[model_name] = model_buffer;
}