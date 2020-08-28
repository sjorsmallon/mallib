#include "render_system.h"
#include <log/log.h>

namespace
{
	unsigned int g_position;
    unsigned int g_normal;
    unsigned int g_albedo_spec;
    unsigned int g_buffer; 

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
        // std::set<GLenum> warning_types_to_ignore{0x8251, 0x8250};

        const bool warning_can_be_ignored = false;

        if (!warning_can_be_ignored) 
        {
            if (type == GL_DEBUG_TYPE_ERROR) logr::report_error( "GL CALLBACK: type = 0x{:x}, severity = 0x{:x}, message = {}\n", type, severity, message);
            else
            {
                logr::report("GL CALLBACK: type = 0x{:x}, severity = 0x{:x}, message = {}\n", type, severity, message);
            }
           // fprintf(
           //      stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           //      ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
           //      type, severity, message );  
        }
    }
    //std::vector<> draw_buffer;
}

// // renderer?
// //@Dependencies:
// // g_buffer
// // g_position
// // g_normal
// // g_albedo_spec
// // rbo_depth
void init_opengl()
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

    // enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_message_callback, 0);

    // DISABLE transparency
    glDisable(GL_BLEND);
    
    // init g_buffer
    glGenFramebuffers(1, &g_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
    
    // - position color buffer
    glGenTextures(1, &g_position);
    glBindTexture(GL_TEXTURE_2D, g_position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);
      
    // - normal color buffer
    glGenTextures(1, &g_normal);
    glBindTexture(GL_TEXTURE_2D, g_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);
      
    // - color + specular color buffer
    glGenTextures(1, &g_albedo_spec);
    glBindTexture(GL_TEXTURE_2D, g_albedo_spec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedo_spec, 0);
      
    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
      
    // then also add render buffer object as depth buffer and check for completeness.
    unsigned int rbo_depth;
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logr::report_error("[OpenGL] Framebuffer is incomplete.\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


static void render()
{
    // render
    // ------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. geometry pass: render scene's geometry/color data into g_buffer
    // -----------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view  = create_view_matrix(camera);
    float aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
    glm::mat4 projection = glm::perspective(glm::radians(fov),aspect_ratio, 0.1f, 100.0f);  
    glm::mat4 model = glm::mat4(1.0f);

    // @TODO: bind_shader("deferred_geometry");
    //        set_uniform_mat4("projection", projection);
    //        set_uniform_mat4("view", view);
    //        auto view = registry.view<renderable>();
    // for (auto entity: view)
    // {
    //      set_uniform_mat4("model". model);
    //      auto& render_data = view.get<renderable>(entity);
    //      draw(render_data.mesh);
    // }
    //      
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the g_buffer's content.
    // -----------------------------------------------------------------------------------------------------------------------
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // bind_shader("deferred_lighting");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_position);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_albedo_spec);


    // send light relevant uniforms
    // for (unsigned int i = 0; i < lightPositions.size(); i++)
    // {

    //     // update_uniform("lights" std::to_string(i) + ".position", light[i].position);
    //     shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
    //     shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
    //     // update attenuation parameters and calculate radius
    //     const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
    //     const float linear = 0.7;
    //     const float quadratic = 1.8;
    //     shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
    //     shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
    //     // then calculate radius of light volume/sphere
    //     const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
    //     float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
    //     shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
    // }
    // shaderLightingPass.setVec3("viewPos", camera.Position);
    // // finally render quad
    // renderQuad();

    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    // ----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the      
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. render lights on top of scene
    // --------------------------------
    // shaderLightBox.use();
    // shaderLightBox.setMat4("projection", projection);
    // shaderLightBox.setMat4("view", view);
    // for (unsigned int i = 0; i < lightPositions.size(); i++)
    // {
    //     model = glm::mat4(1.0f);
    //     model = glm::translate(model, lightPositions[i]);
    //     model = glm::scale(model, glm::vec3(0.125f));
    //     shaderLightBox.setMat4("model", model);
    //     shaderLightBox.setVec3("lightColor", lightColors[i]);
    //     renderCube();
    // }


    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);

}