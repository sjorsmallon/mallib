#include "render_system.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <set>

#include "camera.h"
#include "log.h"
#include "light.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "timed_function.h"

constexpr const int NUM_LIGHTS = 32;
constexpr const unsigned int SHADOW_FB_WIDTH = 1024;
constexpr const unsigned int SHADOW_FB_HEIGHT = 1024;

namespace
{
    // cvars
    int g_window_width;
    int g_window_height;
    float g_aspect_ratio;

    constexpr const float g_fov = 90.0f;
    constexpr const float g_projection_z_near_plane = 0.1f;
    constexpr const float g_projection_z_far_plane = 1000.0f;

    // "members"
    Shader_Manager*   shader_manager;
    Texture_Manager* texture_manager;


    // openGL record keeping

    //--- frame buffers
    unsigned int geometry_fbo; 
    unsigned int depth_rbo;
    //  // shadow
    unsigned int depth_map_fbo;

    //--- textures for framebuffers
    // @IC(Sjors): these are not necessary. They are bound once to the geometry framebuffer.    
    // unsigned int position_tfbo;
    // unsigned int normal_tfbo;
    // unsigned int albedo_specular_tfbo;
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

    unsigned int g_wall_vao;
    unsigned int g_wall_vbo;
    unsigned int g_wall_model_vbo;
    unsigned int g_wall_mvp_vbo;
    std::vector<glm::mat4> g_wall_model_matrices(256);
    std::vector<glm::mat4> g_wall_mvp_matrices(256);

    unsigned int g_debug_geometry_vao;
    unsigned int g_debug_geometry_vbo;
    std::vector<float> g_debug_draw_data;


     // @dependencies:
    // g_debug_draw_buffer
    // world_up
    void put_arrow(const glm::vec3 start_in, const glm::vec3 end_in, const glm::vec3 color_in, const float thickness_in = 1.0f)
    {
        const glm::vec3 world_up = glm::vec3(0.0f,1.0f,0.0f);
        // https://math.stackexchange.com/questions/2563909/find-points-on-a-plane

        const glm::vec3 normal = glm::normalize(end_in - start_in);
        const float distance  = glm::distance(end_in, start_in);

        const glm::vec3 distance_per_axis = end_in - start_in;

        const float epsilon = 0.0001f;

        // plane equation:
        // (Ax + By + Cz + D = 0)
        // new point: (x + 1, y, z + u)
        // new point: (x, y + 1, z + v)
        // assuming C is not 0:
        // μ= −A/C 
        // ν=−B/C

        const float u = -start_in.x / start_in.z;
        const float v = -start_in.y / start_in.z;

        const glm::vec3 first_point{start_in.x + thickness_in, start_in.y, start_in.z + u};
        const glm::vec3 second_point{start_in.x, start_in.y + thickness_in, start_in.z + v};

        // glm::vec3 second_axis = glm::normalize(first_point - start_in);

        glm::vec3 second_axis = glm::cross(normal, world_up);
        glm::vec3 third_axis = glm::cross(normal, second_axis);

        glm::vec3 first_plane_point = start_in + third_axis;
        glm::vec3 second_plane_point = start_in + second_axis;


        // slab
        // stop at 80% of head
        const glm::vec3 slab_end = start_in + (0.8f * distance) * normal; 
        glm::vec3 back_top_left  = start_in + (-0.5f * second_axis) + (0.5f * third_axis);
        glm::vec3 back_top_right = start_in + (0.5f * second_axis) +  (0.5f * third_axis);
        glm::vec3 back_bot_left  = start_in + (-0.5f * second_axis) + (-0.5f * third_axis);
        glm::vec3 back_bot_right = start_in + (0.5f * second_axis) +  (-0.5f * third_axis);


        glm::vec3 front_top_left  =  slab_end +  (-0.5f * second_axis) + (0.5f * third_axis);
        glm::vec3 front_top_right =  slab_end + (0.5f * second_axis) + (0.5f * third_axis);
        glm::vec3 front_bot_left  =  slab_end + (-0.5f * second_axis) + (-0.5f * third_axis);
        glm::vec3 front_bot_right =  slab_end + (0.5f * second_axis) + (-0.5f * third_axis);

        // pyramid on top:
        glm::vec3 head_back_top_left  = slab_end + (-0.8f * second_axis) + (0.8f * third_axis);
        glm::vec3 head_back_top_right = slab_end + (0.8f * second_axis) + (0.8f * third_axis);
        glm::vec3 head_back_bot_left  = slab_end + (-0.8f * second_axis) + (-0.8f * third_axis);
        glm::vec3 head_back_bot_right = slab_end + (0.8f * second_axis) + (-0.8f * third_axis);
        glm::vec3 head_top = end_in;


        std::vector<float> buffer
        {
            // back face
            back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            // front face
            front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            // left
            front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            // right
            front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
             // bottom
            front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            back_bot_right.x, back_bot_right.y, back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            back_bot_left.x, back_bot_left.y, back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            front_bot_left.x, front_bot_left.y, front_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            front_bot_right.x, front_bot_right.y, front_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            // top
            front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,      
            back_top_right.x, back_top_right.y, back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,         
            back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            back_top_left.x, back_top_left.y, back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            front_top_left.x, front_top_left.y, front_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            
            front_top_right.x, front_top_right.y, front_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z            
        };

        std::vector<float> head_buffer{
            // back face
            head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
            head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
            head_back_bot_right.x, head_back_bot_right.y, head_back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
            head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
            head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
            head_back_top_left.x, head_back_top_left.y, head_back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
            // sides of the pyramid: left:
            head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            //
            head_back_top_left.x, head_back_top_left.y, head_back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
            head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
            // rihgt:
            head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,            //
            head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
            head_back_bot_right.x, head_back_bot_right.y, head_back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
            // bot:
            head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,
            head_back_bot_left.x, head_back_bot_left.y, head_back_bot_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,           
            head_back_bot_right.x, head_back_bot_right.y, head_back_bot_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,        
            // top:
            head_top.x, head_top.y, head_top.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,
            head_back_top_left.x, head_back_top_left.y, head_back_top_left.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z,  
            head_back_top_right.x, head_back_top_right.y, head_back_top_right.z, 0.0f, 0.0f, 1.0f, color_in.x, color_in.y,color_in.z
        };
      
        g_debug_draw_data.insert(std::end(g_debug_draw_data), std::begin(buffer), std::end(buffer));
        g_debug_draw_data.insert(std::end(g_debug_draw_data), std::begin(head_buffer), std::end(head_buffer));
    };


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
        }      

        // init Deferred renderer
        // -----------------------------------------------------------
        {
            // init geometry_fbo
            glGenFramebuffers(1, &geometry_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);
            
            // register all three framebuffers as color attachment (GL_COLOR_ATTACHMENTN), N -> {0 : position, 1: normals , 2: albedo & specular}
            
            // - position frame buffer
            uint32_t position_tfbo = register_framebuffer_texture(*texture_manager, "position_tfbo");
            auto& position_texture = texture_manager->textures["position_tfbo"];
            glActiveTexture(GL_TEXTURE0 + position_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, position_tfbo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position_tfbo, 0);
              
            // - normals frame buffer
            uint32_t normal_tfbo = register_framebuffer_texture(*texture_manager, "normal_tfbo");
            auto& normal_texture = texture_manager->textures["normal_tfbo"];
            glActiveTexture(GL_TEXTURE0 + normal_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, normal_tfbo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tfbo, 0);
              
            // - color + specular frame buffer
            uint32_t albedo_specular_tfbo = register_framebuffer_texture(*texture_manager, "albedo_specular_tfbo");
            auto& albedo_specular_texture = texture_manager->textures["albedo_specular_tfbo"];
            glActiveTexture(GL_TEXTURE0 + albedo_specular_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, albedo_specular_tfbo);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo_specular_tfbo, 0);
              
            // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
            unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
            glDrawBuffers(3, attachments);
              
            // then also add render buffer object as depth buffer and check for completeness.
            unsigned int depth_rbo;
            glGenRenderbuffers(1, &depth_rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, frame_buffer_width, frame_buffer_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

            // finally check if framebuffer is complete
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logr::report_error("[OpenGL] Framebuffer is incomplete.\n");

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

        // Shadow Map
        //------------------------------------------------------
        glGenFramebuffers(1, &depth_map_fbo);
        g_depth_map_tfbo = register_framebuffer_texture(*texture_manager, "depth_map_tfbo");
        auto& depth_map_texture = texture_manager->textures["depth_map_tfbo"];
        glActiveTexture(GL_TEXTURE0 + depth_map_texture.gl_texture_frame);
        glBindTexture(GL_TEXTURE_2D, g_depth_map_tfbo);
        glTexImage2D(
            GL_TEXTURE_2D, 
            0, 
            GL_DEPTH_COMPONENT, 
            SHADOW_FB_WIDTH, 
            SHADOW_FB_HEIGHT, 
            0, 
            GL_DEPTH_COMPONENT, 
            GL_FLOAT, 
            NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_depth_map_tfbo, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  
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
        float floor_vertices[]  = {
            -500.0f, -5.0f, 500.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
             500.0f, -5.0f, 500.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -500.0f, -5.0f, -500.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -500.0f, -5.0f, -500.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             500.0f, -5.0f, 500.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             500.0f, -5.0f, -500.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
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

        // add some data to the buffer.
        put_arrow(glm::vec3(1.0f, 0.5f, -2.0f), glm::vec3(1.0f, 5.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        put_arrow(glm::vec3(1.0f, 0.5f, -2.0f), glm::vec3(6.0f, 0.5f, -2.0f), glm::vec3(1.0f, 0.0f, 1.0f));

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) *g_debug_draw_data.size(), g_debug_draw_data.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

}

void init_renderer(Shader_Manager& shader_manager_in, Texture_Manager& texture_manager_in, const int frame_buffer_width, const int frame_buffer_height)
{
    // initialize globals.
    shader_manager = &shader_manager_in;
    texture_manager = &texture_manager_in;

    g_window_width  = frame_buffer_width;
    g_window_height = frame_buffer_height;

    g_aspect_ratio = static_cast<float>(g_window_width) / static_cast<float>(g_window_height);

    init_opengl(frame_buffer_width, frame_buffer_height);
    init_unit_cube();
    init_NDC_quad();
    init_floor();
    init_instanced_wall();
    init_debug_geometry();
}

//@Temporary
void render_back_of_cube()
{
    // render back Cube
    glBindVertexArray(g_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbo);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

//@Temporary
void render_front_of_cube()
{
    // render back Cube
    glBindVertexArray(g_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbo);
    glDrawArrays(GL_TRIANGLES, 6, 6);
    glBindVertexArray(0);
}

void render_cube()
{
    // render Cube
    glBindVertexArray(g_cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_cube_vbo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// render_NDC_quad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void render_NDC_quad()
{  
    glBindVertexArray(g_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_quad_vbo);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void render_floor()
{
    glBindVertexArray(g_floor_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_floor_vbo);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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

        // bind shader and update all relevant uniforms.
        set_shader(*shader_manager, "deferred_geometry");
        set_uniform(*shader_manager, "projection", projection);
        set_uniform(*shader_manager, "view", view);

        // 1.a:  render geometry in the scene.
        // ---------------------------------------
        {
            // for all cubes in the scene..)
            {
                auto& cube_texture = texture_manager->textures["metal"];
                set_uniform(*shader_manager,  "texture_diffuse", cube_texture.gl_texture_frame);

                glm::mat4 model = glm::mat4(1.0f);
                set_uniform(*shader_manager, "model", model);
                render_cube();

            }
       
            // render floor
            {
                auto& marble_texture = texture_manager->textures["marble"];
                set_uniform(*shader_manager, "texture_diffuse",  marble_texture.gl_texture_frame);

                glm::mat4 model = glm::mat4(1.0f);
                set_uniform(*shader_manager, "model", model);
                render_floor();
            }

            // render walls
            {
                timed_function("render_walls");

                set_shader(*shader_manager, "deferred_instanced");
                set_uniform(*shader_manager, "view", view);

                // update matrix of the wall.
                {
                    auto& model_matrix_0 = g_wall_model_matrices[0];
                    auto& mvp_matrix_0 = g_wall_mvp_matrices[0];
                    model_matrix_0 = glm::mat4(1.0f);
                    model_matrix_0 = glm::translate(model_matrix_0, glm::vec3(2.0f, 0.0f, 0.0f));
                    model_matrix_0 = glm::scale(model_matrix_0, glm::vec3(100.0f));
                    mvp_matrix_0 = projection * view * model_matrix_0;  
                }

                glNamedBufferData(g_wall_model_vbo, sizeof(glm::mat4) * g_wall_model_matrices.size(), g_wall_model_matrices.data(), GL_DYNAMIC_DRAW);
                glNamedBufferData(g_wall_mvp_vbo,   sizeof(glm::mat4) * g_wall_mvp_matrices.size(),   g_wall_mvp_matrices.data(), GL_DYNAMIC_DRAW);

                auto& wall_stone_diffuse_texture  = texture_manager->textures["wall_stone_diffuse"];
                auto& wall_stone_specular_texture = texture_manager->textures["wall_stone_specular"];
                auto& wall_stone_normal_texture   = texture_manager->textures["wall_stone_normal"];

                // set_uniform(*shader_manager, "texture_diffuse", wall_stone_diffuse_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_diffuse",  wall_stone_diffuse_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_specular", wall_stone_specular_texture.gl_texture_frame);


                glBindVertexArray(g_wall_vao);
                glDrawArraysInstanced(
                    GL_TRIANGLES,
                    0,
                    6,
                    g_wall_model_matrices.size());
                glBindVertexArray(0);
            }


        }
       
        // unbind geometry buffer, bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // get texture handles for the frame buffers.
    auto& position_tfbo_texture = texture_manager->textures["position_tfbo"];
    auto& normal_tfbo_texture = texture_manager->textures["normal_tfbo"];
    auto& albedo_specular_tfbo_texture = texture_manager->textures["albedo_specular_tfbo"];

    // @Volatile(Sjors): if this changes, the deferred_lighting shader step should change as well.
    std::vector<Light> lights(NUM_LIGHTS);

    // 2. lighting pass:
    // -----------------------------------------------------------------
    {
        set_shader(*shader_manager, "deferred_lighting");

        set_uniform(*shader_manager, "fb_position",    position_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager, "fb_normal",      normal_tfbo_texture.gl_texture_frame);    
        set_uniform(*shader_manager, "fb_albedo_spec", albedo_specular_tfbo_texture.gl_texture_frame);    

        glm::vec4 camera_position = glm::vec4(camera.position, 1.0f); 
        set_uniform(*shader_manager, "view_position", glm::vec4(camera_position));

        // for (auto& light: lights)
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
            lights[0].quadratic = 0.02f;

            lights[1].position = glm::vec4(0.0f, 3.0f, 0.0f, 0.0f);
            lights[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
            lights[1].on = true;
            lights[1].linear = 0.1;
            lights[1].quadratic = 0.2;

        }
        
        //@Note(Sjors): while not necessary to bind the buffer
        // when using glnamedbufferdata to update the buffer. it IS important to still bind it before rendering the
        // NDC quad, since any glDraw* call will take the currently bound array / index buffer.
        glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
        glNamedBufferData(light_ubo, lights.size() * sizeof(Light),  lights.data(), GL_DYNAMIC_DRAW);

        render_NDC_quad();
    }

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


    // // 3. render geometry on top of scene that does not need to be affected by lighting.
    // --------------------------------
    {

        // only render the active lights.normally, for light: active_lights)
        {
            set_shader( *shader_manager, "lightbox");
            set_uniform(*shader_manager, "projection", projection);
            set_uniform(*shader_manager, "view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lights[0].position));
            model = glm::scale(model, glm::vec3(0.01f));
            set_uniform(*shader_manager, "model", model);
            set_uniform(*shader_manager, "light_color", lights[0].color);
            render_cube();

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lights[1].position));
            model = glm::scale(model, glm::vec3(0.125f));
            set_uniform(*shader_manager, "model", model);
            set_uniform(*shader_manager, "light_color", lights[1].color);
            render_cube();

            // model = glm::mat4(1.0f);
            // model = glm::translate(model, glm::vec3(lights[2].position));
            // model = glm::scale(model, glm::vec3(0.125f));
            // set_uniform(*shader_manager, "model", model);
            // set_uniform(*shader_manager, "light_color", lights[2].color);
            // render_cube();

            // model = glm::mat4(1.0f);
            // model = glm::translate(model, glm::vec3(lights[3].position));
            // model = glm::scale(model, glm::vec3(0.125f));
            // set_uniform(*shader_manager, "model", model);
            // set_uniform(*shader_manager, "light_color", lights[3].color);
            // render_cube();

  
        }

        // render debug data
        {
            set_shader(*shader_manager, "deferred_debug_geometry");

            set_uniform(*shader_manager, "view", view);
            set_uniform(*shader_manager, "projection", projection);
            set_uniform(*shader_manager, "model", glm::mat4(1.0f));

            glBindVertexArray(g_debug_geometry_vao);
            glBindBuffer(GL_ARRAY_BUFFER, g_debug_geometry_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) *g_debug_draw_data.size(), g_debug_draw_data.data(), GL_STATIC_DRAW);

            glDrawArrays(GL_TRIANGLES, 0, g_debug_draw_data.size() / 9);
        }

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

                // render_floor();
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

            auto& diffuse_texture = texture_manager->textures["metal"];
            auto& shadow_map = texture_manager->textures["depth_map_tfbo"];
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


//@Note(Sjors)light falloff calculation.

// 1.0/(1.0 + c1*d + c2*d^2)

//  // //     //     // update attenuation parameters and calculate radius
//  //         // const float constant = 1.0f;
//  //         // const float linear = 0.7f;
//  //         // const float quadratic = 1.8f;
//  // //     //     shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
//  // //     //     shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
//  // //     //     // then calculate radius of light volume/sphere
//  // //     //     const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
//  // //     //     float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
//  // //     //     shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
//  //     }


// read framebuffer
// GLint drawFboId = 0, readFboId = 0;
// glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
// glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);
// logr::report("current draw framebuffer bound: {}.\n current  read framebuffer bound: {}\n", drawFboId, readFboId);