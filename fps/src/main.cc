// Include glfw3.h after our OpenGL definitions
#include "window_manager.h"


#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <log/log.h>
#include <entt/entt.h>
#include "camera.h"
#include "entity_system.h"
#include "render_system.h"

#include "shader_manager.h"
#include "texture_manager.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// input handling
static double last_x = 0.0;
static double last_y = 0.0;
static double delta_x = 0.0;
static double delta_y = 0.0;
static double scroll_delta_y = 0.0;
static bool holding_left_mouse_button = false;
static bool holding_right_mouse_button = false;
// input handling - settings
static double mouse_sensitivity = 0.0005;
static float fov = 90.0f;

// window - settings
const int window_width = 1920;
const int window_height = 1080;
const int debug_window_width = 850;
const int debug_window_height = 1000;
bool vsync_enabled = true;

// opengl
Camera camera{};

void glfw_error_callback(int error, const char* description);
void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// void update(float frame_dt)?


int main()
{
    GLFWwindow* main_window = glfw_init_window("mvmt_fps", window_width, window_height);
    // GLFWwindow* debug_window = glfw_init_debug_window(debug_window_width, debug_window_height, main_window);

    glfwMakeContextCurrent(main_window);
    // register callbacks
    glfwSetErrorCallback(glfw_error_callback);
    glfwSetKeyCallback(main_window, glfw_key_callback);
    glfwSetMouseButtonCallback(main_window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(main_window, glfw_cursor_position_callback);
    glfwSetScrollCallback(main_window, glfw_scroll_callback);


    // at this point, no user-defined things have been allocated or loaded. 
    Shader_Manager shader_manager{};
    load_shader(shader_manager, "deferred_geometry");
    load_shader(shader_manager, "deferred_lighting");
    load_shader(shader_manager, "lightbox");
    load_shader(shader_manager, "simple_depth");

    Texture_Manager texture_manager{};
    load_png_texture(texture_manager, "metal");
    init_renderer(shader_manager, texture_manager, window_width, window_height);


    Entity_Manager entity_manager{};
    create_player(entity_manager);
    // create_lights(entity_manager);

    float frame_dt = 0.0f;
    float last_frame_time = 0.0f;

    // Main loop
    while (!glfwWindowShouldClose(main_window))
    {
        // per-frame time logic
        // --------------------
        float current_frame_time = glfwGetTime();
        frame_dt = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;
      
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        render();
        glfwSwapBuffers(main_window);
        // render();

        // render_ui();

        // bool show_ui = true;
        // if (show_ui)
        // {
        //     glfwMakeContextCurrent(imgui_window);
        //     // Start the Dear ImGui frame
        //     ImGui_ImplOpenGL3_NewFrame();
        //     ImGui_ImplGlfw_NewFrame();
        //     ImGui::NewFrame();

        //     {
        //         ImGui::Begin("Active Model");                      
        //         ImGui::End();

        //         ImGui::Begin("Active Camera");
        //         ImGui::End();
        //     }

        //     ImGui::Render();
        //     int display_w, display_h;
        //     glfwGetFramebufferSize(window, &display_w, &display_h);
        //     glViewport(0, 0, display_w, display_h);

        //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //     glfwSwapBuffers(imgui_window);
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // }

    }
    //@todo: delete glfw.
    logr::report("exiting...");

    return 0;
}
 

//@IC(Sjors): parameters cannot be const since the callbacks needs to match.
void glfw_error_callback(int error, const char* description)
{
    logr::report_error("[GLFW]: {}, {} \n", error, description);
}

   
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfw_exit_and_cleanup(window);

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        // move the player, reconstitute camera?
        // auto [position, velocity] = registry.get<position, velocity>(player);
        // position += camera.front * velocity; 
        // logr::report("Moving forwards.\n");
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        // // move the player, reconstitute camera?
        // auto [pos, vel] = registry.get<position, velocity>(player);
        // pos -= camera.front * vel; 
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        // // move the player, reconstitute camera?
        // auto [pos, vel] = registry.get<position, velocity>(player);
        // pos -= camera.right * vel; 
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        // // move the player, reconstitute camera?
        // auto [pos, vel] = registry.get<position, velocity>(player);
        // pos += camera.right * vel; 
    }
}

//@IC(Sjors): parameters cannot be const since the callbacks needs to match.
//@Dependencies: camera.
void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    double delta_x = xpos - last_x;
    double delta_y = last_y - ypos;     
    // update
    last_x = xpos;
    last_y = ypos;

    // update_camera_with_mouse_input(camera, delta_x, delta_y);
}

//@Dependencies: gl call
void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//@Dependencies:
// holding_left_mouse_button,
// holding_right_mouse_button
void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            // logr::report("left mouse button clicked.\n");
            // holding_left_mouse_button = true;
        }
        else if (action == GLFW_RELEASE)
        {
            // holding_left_mouse_button = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            // holding_right_mouse_button = true;
        }
        else if (action == GLFW_RELEASE)
        {
            // holding_right_mouse_button = false;
        }
    }
    }

//@dependencies:
// scroll_delta_y
void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // scroll_delta_y = yoffset;
}


