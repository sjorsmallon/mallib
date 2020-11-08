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

constexpr const int NUM_LIGHTS = 32;

namespace
{
    // cvars
    int g_window_width;
    int g_window_height;
    float g_aspect_ratio;
    float g_fov = 90.0f;

    constexpr const float g_projection_z_near_plane = 0.1f;
    constexpr const float g_projection_z_far_plane = 200.0f;

    Shader_Manager*   shader_manager;
    Texture_Manager* texture_manager;

    // openGL record keeping
	unsigned int position_tfbo;
    unsigned int normal_tfbo;
    unsigned int albedo_specular_tfbo;

    unsigned int g_depth_map_tfbo;

    unsigned int geometry_fbo; 
    unsigned int depth_rbo;

    unsigned int g_cube_vao;
    unsigned int g_cube_vbo;

    unsigned int g_quad_vao;
    unsigned int g_quad_vbo;

    unsigned int g_floor_vao;
    unsigned int g_floor_vbo;

    // uniform buffers
    unsigned int light_ubo;

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
         // set clear color
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
         // Enable use of the z-buffer
        glEnable(GL_DEPTH_TEST);    
        // Default is GL_LESS
        glDepthFunc(GL_LEQUAL);

        // Line width 2.0 pixels
        glLineWidth(1.0);
        glEnable(GL_LINE_SMOOTH);

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


        // enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(opengl_message_callback, 0);

        // DISABLE transparency
        glDisable(GL_BLEND);
        

        // Deferred renderer
        // -----------------------------------------------------------

        // init geometry_fbo
        glGenFramebuffers(1, &geometry_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, geometry_fbo);
        
        // register all three framebuffers as color attachment (GL_COLOR_ATTACHMENTN), N -> {0 : position, 1: normals , 2: albedo & specular}
        
        // - position frame buffer
        // glGenTextures(1, &position_tfbo);
        uint32_t position_tfbo = register_framebuffer_texture(*texture_manager, "position_tfbo");
        glBindTexture(GL_TEXTURE_2D, position_tfbo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position_tfbo, 0);
          
        // - normals frame buffer
        // glGenTextures(1, &normal_tfbo);
        uint32_t normal_tfbo = register_framebuffer_texture(*texture_manager, "normal_tfbo");

        glBindTexture(GL_TEXTURE_2D, normal_tfbo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, frame_buffer_width, frame_buffer_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tfbo, 0);
          
        // - color + specular frame buffer
        // glGenTextures(1, &albedo_specular_tfbo);
        uint32_t albedo_specular_tfbo = register_framebuffer_texture(*texture_manager, "albedo_specular_tfbo");
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
        // bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // init uniform buffers
        //@Volatile: this should match both the size and the binding index of the buffer.
        const int LIGHT_BUFFER_UNIFORM_IDX = 2;
        const int light_buffer_size = NUM_LIGHTS * sizeof(Light);

        // glGenBuffers(1, &light_ubo);
        glCreateBuffers(1, &light_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);
        // glBufferData(GL_UNIFORM_BUFFER, light_buffer_size, NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
        glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BUFFER_UNIFORM_IDX, light_ubo); 
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Shadow Map
        //------------------------------------------------------
        // const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        // g_depth_map_tfbo = register_framebuffer_texture(*texture_manager, "g_depth_map_tfbo");
        // glBindTexture(GL_TEXTURE_2D, g_depth_map_tfbo);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
        //              SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

        // glBindFramebuffer(GL_FRAMEBUFFER, depth_);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        // glDrawBuffer(GL_NONE);
        // glReadBuffer(GL_NONE);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    }
// 
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
        


        // link vertex attributes (position, normals, texture coordinates);
        glBindVertexArray(g_cube_vao);
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



// @dependencies:
// cvars: g_fov, g_aspect_ratio, g_projection_z_near_plane, g_projection_z_far_plane
/// - all lights
/// - the geometry that needs to be rendered (either via submission or other)
/// - all draw commands that are issued from all parts.
/// - the texture data (or at least the bindings)
/// - particles
void render(const Camera camera, Particle_Cache& particle_cache)
{
    // render
    // ------
    Camera player_camera = camera;
    glm::mat4 view       = create_view_matrix_from_camera(player_camera);
    glm::mat4 projection = glm::perspective(glm::radians(g_fov), g_aspect_ratio, g_projection_z_near_plane, g_projection_z_far_plane);  

    // logr::report("camera.position: {}\n, camera.front:{}\n", glm::to_string(camera.position), glm::to_string(camera.front));

    // STEP 0: SHADOW MAPPING
    {
        // 0.1: first render to depth map
        // ------------------------------------------
        // {
        //     set_shader(*shader_manager, "simple_depth");

        //     glViewport(0, 0, 1024, 1024);
        //     glBindFramebuffer(GL_FRAMEBUFFER, g_depth_map_tfbo);
        //     glClear(GL_DEPTH_BUFFER_BIT);
        //     float near_plane = 1.0f, far_plane = 7.5f;
        //     // frustum?
        //     glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        //     glm::mat4 light_view =       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), 
        //                                              glm::vec3(0.0f, 3.0f,  0.0f), 
        //                                              glm::vec3(0.0f, 1.0f,  0.0f));  
        //     glm::mat4 light_space_matrix = light_projection * light_view;


        //     set_uniform(*shader_manager, "light_space_matrix", light_space_matrix);
        //     // for all cubes in the scene..)
        //     {
        //         glm::mat4 model      = glm::mat4(1.0f);
        //         set_uniform(*shader_manager, "model", model);
        //         render_cube();
        //     }
        //     // unbind framebuffer
        //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // }

        // // 0.2. then render scene as normal with shadow mapping (using depth map)
        // {
        //     glViewport(0, 0, g_window_width, g_window_height);
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //     glBindTexture(GL_TEXTURE_2D, g_depth_map_tfbo);

        // }
    }
   

    // // 1. geometry pass: render scene's geometry/color data into geometry_fbo
    // // -----------------------------------------------------------------
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

        //@FIXME(Sjors):why do we not bind texture specular1? I don't understand..
        // set_uniform(*shader_manager,  "texture_specular1", );

        // 1.a:  render geometry in the scene.
        // ---------------------------------------
        {
            // for all cubes in the scene..)
            {
                auto& cube_texture = texture_manager->textures["metal"];
                glActiveTexture(GL_TEXTURE0 + cube_texture.gl_texture_frame);
                glBindTexture(GL_TEXTURE_2D, cube_texture.gl_texture_id);
                set_uniform(*shader_manager,  "texture_diffuse1", cube_texture.gl_texture_frame);

                glm::mat4 model = glm::mat4(1.0f);
                set_uniform(*shader_manager, "model", model);
                render_cube();
            }
       
            // render floor
            {
                auto& marble_texture = texture_manager->textures["marble"];
                glActiveTexture(GL_TEXTURE0 + marble_texture.gl_texture_frame);
                glBindTexture(GL_TEXTURE_2D, marble_texture.gl_texture_id);
                set_uniform(*shader_manager, "texture_diffuse1",  marble_texture.gl_texture_frame);
                glm::mat4 model = glm::mat4(1.0f);
                set_uniform(*shader_manager, "model", model);
                render_floor();
            }

        }
       
        // unbind geometry buffer, bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // get texture handles for the frame buffers.
    auto& position_tfbo_texture = texture_manager->textures["position_tfbo"];
    auto& normal_tfbo_texture   = texture_manager->textures["normal_tfbo"];
    auto& albedo_specular_tfbo_texture = texture_manager->textures["albedo_specular_tfbo"];

    // @VOLATILE: if this changes, the deferred_lighting shader step should change as well.
    std::vector<Light> lights(NUM_LIGHTS);

    // 2. lighting pass:
    // -----------------------------------------------------------------
    {
        set_shader(*shader_manager, "deferred_lighting");

        //@TODO(Sjors): do we need to rebind the framebuffer textures here?
        glActiveTexture(GL_TEXTURE0 + position_tfbo_texture.gl_texture_frame);
        glBindTexture(  GL_TEXTURE_2D,  position_tfbo_texture.gl_texture_id);

        glActiveTexture(GL_TEXTURE0 + normal_tfbo_texture.gl_texture_frame);
        glBindTexture(  GL_TEXTURE_2D,  normal_tfbo_texture.gl_texture_id);

        glActiveTexture(GL_TEXTURE0 + albedo_specular_tfbo_texture.gl_texture_frame);
        glBindTexture(  GL_TEXTURE_2D,  albedo_specular_tfbo_texture.gl_texture_id);

        set_uniform(*shader_manager, "fb_position",    position_tfbo_texture.gl_texture_frame);
        set_uniform(*shader_manager, "fb_normal",      normal_tfbo_texture.gl_texture_frame);    
        set_uniform(*shader_manager, "fb_albedo_spec", albedo_specular_tfbo_texture.gl_texture_frame);    


        glm::mat4 camera_model = glm::mat4(1.0f);
        camera_model = glm::translate(camera_model, glm::vec3(camera.position));

        glm::vec4 camera_position = view * camera_model * glm::vec4(camera.position, 1.0f); 
        logr::report("view position: {}\n", glm::to_string(camera_position));
        
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
            lights[0].position.x = player_camera.position.x;
            lights[0].position.y = player_camera.position.y;
            lights[0].position.z = player_camera.position.z;
            lights[0].color = glm::vec4(1.0f,1.0f,1.0f,0.0f);
            lights[0].on = false;
            lights[0].linear = 0.001;
            lights[0].quadratic = 0.001;

            lights[1].position = glm::vec4(0.0f, 3.0f, 0.0f, 0.0f);
            lights[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
            lights[1].on = true;
            lights[1].linear = 0.1;
            lights[1].quadratic = 0.2;

            lights[2].position.x = x_position;
            lights[2].position.y = y_position;
            lights[2].position.z = -1.5f;
            lights[2].color = glm::vec4(1.0f,0.0f,1.0f,0.0f);
            lights[2].on = true;
            lights[2].linear = 0.1;
            lights[2].quadratic = 0.2;

            lights[3].position.x = x_position;      
            lights[3].position.y = y_position;
            lights[3].position.z = 1.5f;
            lights[3].color = glm::vec4(1.0f,1.0f,1.0f,0.0f);
            lights[3].on = true;
            lights[3].linear = 0.1;
            lights[3].quadratic = 0.2;

        }
        
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


    // // 3. render lights on top of scene
    // --------------------------------
    {

        // only render the active lights.normally, for light: active_lights)
        {
            set_shader( *shader_manager, "lightbox");
            set_uniform(*shader_manager, "projection", projection);
            set_uniform(*shader_manager, "view", view);

          

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lights[1].position));
            model = glm::scale(model, glm::vec3(0.125f));
            set_uniform(*shader_manager, "model", model);
            set_uniform(*shader_manager, "light_color", lights[1].color);
            render_cube();

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lights[2].position));
            model = glm::scale(model, glm::vec3(0.125f));
            set_uniform(*shader_manager, "model", model);
            set_uniform(*shader_manager, "light_color", lights[2].color);
            render_cube();

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lights[3].position));
            model = glm::scale(model, glm::vec3(0.125f));
            set_uniform(*shader_manager, "model", model);
            set_uniform(*shader_manager, "light_color", lights[3].color);
            render_cube();

            // model = glm::mat4(1.0f);
            // model = glm::translate(model, glm::vec3(lights[3].position));
            // model = glm::scale(model, glm::vec3(0.125f));
            // set_uniform(*shader_manager, "model", model);
            // set_uniform(*shader_manager, "light_color", lights[3].color);
            // render_cube();
        }
    }

}

// 1.0/(1.0 + c1*d + c2*d^2)


//@Note(Sjors)light falloff calculation.

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