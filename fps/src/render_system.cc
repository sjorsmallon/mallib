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

#include "math.h" // M_PI

const int INVALID_VAO = -1;
const int INVALID_VBO = -1;
const int INVALID_UBO = -1;

const int NONE_VAO = 0;
const int NONE_VBO = 0;
const int NONE_UBO = 0;

const int DEFAULT_FRAMEBUFFER_ID = 0;
const int NUM_LIGHTS = 32;

unsigned int DEBUG_POSITION_VAO = INVALID_VAO;
unsigned int DEBUG_POSITION_VBO = INVALID_VBO;


struct gl_state_t
{
    int32_t max_texture_image_units;
    int32_t max_feedback_buffers;
    int32_t max_shader_storage_block_size;
    int32_t max_fragment_texture_image_units;
    int32_t max_uniform_block_size;
    int32_t max_vertex_uniform_blocks;
    int32_t max_geometry_uniform_blocks;
    int32_t max_fragment_uniform_blocks;
    int32_t max_frame_buffer_color_attachments;
};

struct Model_Buffer
{
    unsigned int VAO;
    unsigned int VBO;
    unsigned int model_matrix_VBO;
    size_t vertex_count;
    std::vector<glm::mat4> model_matrix_buffer;
};

namespace
{
    //information about gl. 
    gl_state_t f_gl_state;

    // draw_request
    std::vector<Draw_Request> f_debug_draw_requests;

    std::map<std::string, Model_Buffer> f_model_buffers;

    // cvars
    int g_window_width = 1920;
    int g_window_height = 1080;
    float g_aspect_ratio = static_cast<float>(g_window_width) / static_cast<float>(g_window_height);

    const float g_fov = 110.0f;
    const float g_projection_z_near_plane = 0.1f;
    const float g_projection_z_far_plane = 8192.0f;

    // "members"
    Shader_Manager*   shader_manager;
    Texture_Manager*  texture_manager;
    Asset_Manager*    asset_manager;
    Entity_Manager*   entity_manager;

    // openGL record keeping

    //--- frame buffers
    unsigned int geometry_fbo; 
    unsigned int depth_rbo;
    // shadow
    unsigned int depth_map_fbo;
    // shadow texture buffer
    unsigned int g_depth_map_tfbo;

    //--- textures for framebuffers
    // @IC(Sjors): these are not necessary. They are bound once to the deferred geometry framebuffer.    
    // unsigned int position_tfbo;
    // unsigned int normal_tfbo;
    // unsigned int albedo_specular_tfbo;
    // unsigned int roughness_tfbo;
    // unsigned int metallic_tfbo;
    // unsigned int ambient_occlusion_tfbo;
    // unsigned int tnormal_tfbo;

    //--- uniform buffers
    unsigned int light_ubo;

    unsigned int g_screen_quad_vao;
    unsigned int g_screen_quad_vbo;

    unsigned int g_hud_vao;
    unsigned int g_hud_vbo;

    unsigned int g_viewmodel_vao;
    unsigned int g_viewmodel_vbo;
    
    size_t g_viewmodel_vertex_count;

    unsigned int g_floor_vao;
    unsigned int g_floor_vbo;


    unsigned int g_wall_vao;
    unsigned int g_wall_vbo;
    unsigned int g_wall_model_vbo;
    unsigned int g_wall_mvp_vbo;
    std::vector<glm::mat4> g_wall_model_matrices(256);
    std::vector<glm::mat4> g_wall_mvp_matrices(256);


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
        //----modify global openGL state
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
    
        // query openGL state
        {
            gl_state_t gl_state;
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &gl_state.max_texture_image_units);
            logr::report("[OpenGL] max combined texture image units: {}\n", gl_state.max_texture_image_units);

            glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &gl_state.max_feedback_buffers);
            logr::report("[OpenGL] max feedback buffers: {}\n", gl_state.max_feedback_buffers);

            glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &gl_state.max_shader_storage_block_size);
            logr::report("[OpenGL] max shader storage block size: {}\n", gl_state.max_shader_storage_block_size);

            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_state.max_fragment_texture_image_units);
            logr::report("[OpenGL] max fragment shader texture units: {}\n", gl_state.max_fragment_texture_image_units);

            glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gl_state.max_uniform_block_size);
            logr::report("[OpenGL] max uniform block size: {}\n", gl_state.max_uniform_block_size);

            glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &gl_state.max_vertex_uniform_blocks);
            logr::report("[OpenGL] max Vertex uniform blocks: {}\n", gl_state.max_vertex_uniform_blocks);

            glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &gl_state.max_geometry_uniform_blocks);
            logr::report("[OpenGL] max Geometry uniform blocks: {}\n", gl_state.max_geometry_uniform_blocks);

            glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &gl_state.max_fragment_uniform_blocks);
            logr::report("[OpenGL] max Fragment uniform blocks: {} \n", gl_state.max_fragment_uniform_blocks);

            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &gl_state.max_frame_buffer_color_attachments);
            logr::report("[OpenGL] max fragment uniform blocks: {}\n", gl_state.max_fragment_uniform_blocks);   

            // assign it to the file scoped variable. We could use the f_gl_state immediately,
            // but I may want to move this around without too much hassle;
            f_gl_state = gl_state;         
        }      

        // init Deferred renderer context (framebuffer)
        // order:
        // 0: position,
        // 1: vertex normal
        // 2: diffuse / specular,
        // 3: roughness
        // 4: metallic
        // 5: ambient_occlusion
        // 6: displacement
        // 7: texture_normal
        // -----------------------------------------------------------
        {
            // init geometry_fbo
            glGenFramebuffers(1, &geometry_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);
         
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
              
            // - roughness frame buffer
            uint32_t roughness_tfbo = register_framebuffer_texture(*texture_manager, "roughness_tfbo"); 
            auto& roughness_texture = get_texture(*texture_manager, "roughness_tfbo");

            glActiveTexture(GL_TEXTURE0 + roughness_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, roughness_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, roughness_tfbo, 0);

            // -  metallic frame buffer.
            uint32_t metallic_tfbo = register_framebuffer_texture(*texture_manager, "metallic_tfbo");
            auto& metallic_texture = get_texture(*texture_manager, "metallic_tfbo");

            glActiveTexture(GL_TEXTURE0 + metallic_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, metallic_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, metallic_tfbo, 0);

            // - ambient occlusion frame buffer.
            uint32_t ambient_occlusion_tfbo = register_framebuffer_texture(*texture_manager, "ambient_occlusion_tfbo");
            auto& ambient_occlusion_texture = get_texture(*texture_manager, "ambient_occlusion_tfbo");

            glActiveTexture(GL_TEXTURE0 + ambient_occlusion_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, ambient_occlusion_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, ambient_occlusion_tfbo, 0);

            // -  displacement frame buffer
            uint32_t displacement_tfbo = register_framebuffer_texture(*texture_manager, "displacement_tfbo");
            auto& displacement_texture = get_texture(*texture_manager, "displacement_tfbo");

            glActiveTexture(GL_TEXTURE0 + displacement_texture.gl_texture_frame);
            glBindTexture(GL_TEXTURE_2D, displacement_tfbo);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, displacement_tfbo, 0);

            // - texture normal
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
            assert((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) && "glCheckFrameBufferStatus: deferred framebuffer is incomplete.");

            // unbind geometry framebuffer default framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAMEBUFFER_ID);
        }


        // init uniform buffers
        {
            // light buffer
            {
                //@Volatile(Sjors): this should match both the size and the binding index of the buffer in ALL shaders.
                const int LIGHT_BUFFER_UNIFORM_IDX = 2;
                const int light_buffer_size = NUM_LIGHTS * sizeof(Light);
                glCreateBuffers(1, &light_ubo);
                glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
                glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BUFFER_UNIFORM_IDX, light_ubo); 
                glBindBuffer(GL_UNIFORM_BUFFER, NONE_UBO);
            }
        }
    }

    //@Once
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

    //@Once
    //@NOTE: NDC is -1,-1 to 1,1. middle is thus 0.0
    void init_hud()
    {
        //@Fixme(Sjors): formalize this.
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

    //@Once
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
        glGenVertexArrays(1, &g_screen_quad_vao);
        glGenBuffers(1, &g_screen_quad_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_screen_quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
        glBindVertexArray(g_screen_quad_vao);

        // init vertex attributes. {0: position (xyz), 1: texture coords (uv)}
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    //@Once
    void init_debug_buffers()
    {
        glGenVertexArrays(1, &DEBUG_POSITION_VAO);
        glBindVertexArray(DEBUG_POSITION_VAO);
        glGenBuffers(1, &DEBUG_POSITION_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, DEBUG_POSITION_VBO);

        const int32_t position_attribute_id = 0;
        const int32_t position_float_count  = 3;
        const int32_t position_byte_offset  = 0; // 0
        const int32_t vertex_float_count    = position_float_count;

        constexpr int32_t vertex_byte_stride = vertex_float_count * sizeof(float); 

        glEnableVertexAttribArray(position_attribute_id);
        glVertexAttribPointer(
            position_attribute_id,
            position_float_count,
            GL_FLOAT,
            GL_FALSE,
            vertex_byte_stride,
            (void*)position_byte_offset
        );

        glBindBuffer(GL_ARRAY_BUFFER, NONE_VBO);
        glBindVertexArray(NONE_VAO);
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

        const int32_t vertex_float_count    = position_float_count + normal_float_count + uv_float_count; // 8

        constexpr int32_t vertex_byte_stride = vertex_float_count * sizeof(float); 

        // set vertex count.
        model_buffer.vertex_count = interleaved_XNU_values.size() / vertex_float_count; 

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


        // allocate uniform buffer for model matrices.
        {
            //@Memory(Sjors):
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

            const int32_t row_count   = 4;
            const int row_float_count = 4;
            const int matrix_byte_stride = sizeof(glm::mat4);
            const int row_byte_stride    = sizeof(glm::vec4);

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
}

void submit_debug_draw_request(Draw_Request& draw_request)
{
    f_debug_draw_requests.push_back(draw_request);
}


void init_render_system(
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
    init_NDC_quad();
    init_hud();
    init_debug_buffers();

    //@cleanup: how to do this?
    auto& dodecahedron_obj = get_obj(*asset_manager, "dodecahedron");
    create_interleaved_XNU_model_buffer("dodecahedron", dodecahedron_obj.interleaved_XNU, 128);

}



// render_NDC_quad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void render_NDC_quad()
{  
    glBindVertexArray(g_screen_quad_vao);
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

    Camera player_camera = camera;
    glm::mat4 view       = create_view_matrix_from_camera(player_camera);
    glm::mat4 projection = glm::perspective(glm::radians(g_fov), g_aspect_ratio, g_projection_z_near_plane, g_projection_z_far_plane);  

    // 1a geometry pass: render static geometry/color data into geometry_fbo
    // -----------------------------------------------------------------
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind the geometry buffer as the frame buffer.
        glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deferred_PBR
        // ------------------------------------------
        {
            // render static geometry.

            // render dynamic geometry.

            // dynamic: render dodecahedron
            {
                set_shader(*shader_manager, "deferred_instanced_pbr_no_mvp");

                set_uniform(*shader_manager, "projection", projection);
                set_uniform(*shader_manager, "view", view);
                const auto& moss_albedo_texture = get_texture(*texture_manager, "moss_2K_color");
                const auto& moss_normal_texture = get_texture(*texture_manager, "moss_2K_normal");
                const auto& moss_roughness_texture = get_texture(*texture_manager, "moss_2K_roughness");
                //const auto& moss_metallic_texture does not exist :^)
                const auto& moss_ambient_occlusion_texture = get_texture(*texture_manager, "moss_2K_ambient_occlusion");
                const auto& moss_displacement_texture = get_texture(*texture_manager, "moss_2K_displacement");

                set_uniform(*shader_manager, "texture_albedo", moss_albedo_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_normal", moss_normal_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_roughness", moss_roughness_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_metallic", moss_roughness_texture.gl_texture_frame); // this does not exist.
                set_uniform(*shader_manager, "texture_ambient_occlusion", moss_ambient_occlusion_texture.gl_texture_frame);
                set_uniform(*shader_manager, "texture_displacement", moss_displacement_texture.gl_texture_frame);

                auto& model_buffer = f_model_buffers["dodecahedron"];

                size_t entity_count = 0;
                for (auto&& entity: by_type(*entity_manager, Entity_Type::Cube))
                {
                    if (entity_count >= model_buffer.model_matrix_buffer.size()) logr::report_warning("woops. index out of range!\n");
                    auto& model_matrix = model_buffer.model_matrix_buffer[entity_count];

                    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
                    glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);
                    glm::mat4 rotate = glm::mat4(1.0f);
                    model_matrix = translate * rotate * scale;

                    entity_count += 1;
                }
                glNamedBufferData(
                    model_buffer.model_matrix_VBO, 
                    sizeof(glm::mat4) * model_buffer.model_matrix_buffer.size(), 
                    model_buffer.model_matrix_buffer.data(), 
                    GL_DYNAMIC_DRAW);

                glBindVertexArray(model_buffer.VAO);
                glDrawArraysInstanced(
                    GL_TRIANGLES,
                    0,
                    model_buffer.vertex_count,
                    entity_count
                    );
                glBindVertexArray(NONE_VAO);
            }


        }
       
        // unbind geometry buffer, bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    // @Volatile(Sjors): if this changes, the deferred_lighting shader step should change as well.
    std::vector<Light> lights(NUM_LIGHTS);

    //  2.post deferred PBR lighting pass:
    { 
        const auto& position_tfbo_texture =          get_texture(*texture_manager, "position_tfbo");
        const auto& normal_tfbo_texture =            get_texture(*texture_manager, "normal_tfbo");
        const auto& albedo_specular_tfbo_texture =   get_texture(*texture_manager, "albedo_specular_tfbo");
        const auto& roughness_tfbo_texture=          get_texture(*texture_manager, "roughness_tfbo");
        const auto& metallic_tfbo_texture =          get_texture(*texture_manager, "metallic_tfbo");
        const auto& ambient_occlusion_tfbo_texture = get_texture(*texture_manager, "ambient_occlusion_tfbo");
        const auto& displacement_tfbo_texture =      get_texture(*texture_manager, "displacement_tfbo");
        const auto& tnormal_tfbo_texture =           get_texture(*texture_manager, "tnormal_tfbo");

        set_shader(*shader_manager, "post_deferred_pbr");

        set_uniform(*shader_manager,"fb_position",          position_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager,"fb_normal",            normal_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager,"fb_albedo_spec",       albedo_specular_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager,"fb_roughness",         roughness_tfbo_texture.gl_texture_frame);
        // set_uniform(*shader_manager,"fb_metallic",       metallic_tfbo_texture.gl_texture_frame); // disabled
        set_uniform(*shader_manager,"fb_ambient_occlusion", ambient_occlusion_tfbo_texture.gl_texture_frame);
        // set_uniform(*shader_manager,"fb_displacement",   displacement_tfbo_texture.gl_texture_frame); // disabled
        set_uniform(*shader_manager,"fb_tnormal",           tnormal_tfbo_texture.gl_texture_frame);

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


    // // // 3. render geometry on top of scene that does not need to be affected by lighting.
    // // --------------------------------
   {
         // render debug draw requests.
        {
            // bind shader:
            set_shader(*shader_manager,  "debug_position_world"); // expects 1:1 world space.

            //@Note(sjors): no model matrix needed /expected
            set_uniform(*shader_manager, "model", glm::mat4(1.f));
            set_uniform(*shader_manager, "view", view);
            set_uniform(*shader_manager, "projection", projection);

            // bind VAO 
            glBindVertexArray(DEBUG_POSITION_VAO);

            for (auto&& draw_request: f_debug_draw_requests)
            {
                // update uniforms.
                set_uniform(*shader_manager, "color", draw_request.color);
                set_uniform(*shader_manager, "highlight_color", draw_request.highlight_color);

                const int buffer_byte_size = draw_request.vertices.size() * sizeof(glm::vec3);
                
                if (draw_request.wireframe)
                {
                    glLineWidth(draw_request.line_width);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                // upload data to buffer.
                glNamedBufferData(DEBUG_POSITION_VBO, buffer_byte_size, draw_request.vertices.data(), GL_DYNAMIC_DRAW);

                //draw the actual thing.
                glDrawArrays(draw_request.primitive, 0, draw_request.vertices.size());
            }

            //@FIXME(Sjors): end-of-frame clear / memset to zero.
            f_debug_draw_requests.clear();

            // unbind vertex array and shader.
            glBindVertexArray(NONE_VAO);
            set_shader(*shader_manager, "none");
        }

        // post step 3: restore default settings.
        glLineWidth(2.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }
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