#include "window_manager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <log/log.h>

namespace
{
    // //@IC(Sjors): parameters cannot be const since the callbacks needs to match.
    // void glfw_error_callback(int error, const char* description)
    // {
    //     logr::report_error("[GLFW]: {}, {} \n", error, description);
    // }

    // // openGL error callback
    // void GLAPIENTRY opengl_message_callback(
    //     GLenum source,
    //     GLenum type,
    //     GLuint id,
    //     GLenum severity,
    //     GLsizei length,
    //     const GLchar* message,
    //     const void* userParam )
    // {
    //     /// these warning IDs are ignored.
    //     /// 0x8251: bound to video memory (intended.)
    //     /// 0x8250: buffer performance warning: copying atomic buffer (which we want to do, since we need the values.)
    //     // std::set<GLenum> warning_types_to_ignore{0x8251, 0x8250};

    //     const bool warning_can_be_ignored = false;

    //     if (!warning_can_be_ignored) 
    //     {
    //         fprintf(
    //             stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    //             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
    //             type, severity, message );  
    //     }

    // }

    // void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    // {
    //     if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //         glfw_exit_and_cleanup(window);

    //     if (key == GLFW_KEY_W && action == GLFW_PRESS)
    //     {
    //         // // move the player, reconstitute camera?
    //         // auto [position, velocity] = registry.get<position, velocity>(player);
    //         // position += camera.front * velocity; 
    //         logr::report("Moving forwards.\n");
            
    //     }
    //     if (key == GLFW_KEY_S && action == GLFW_PRESS)
    //     {
    //         // // move the player, reconstitute camera?
    //         // auto [pos, vel] = registry.get<position, velocity>(player);
    //         // pos -= camera.front * vel; 
    //     }

    //     if (key == GLFW_KEY_A && action == GLFW_PRESS)
    //     {
    //         // // move the player, reconstitute camera?
    //         // auto [pos, vel] = registry.get<position, velocity>(player);
    //         // pos -= camera.right * vel; 
    //     }
    //     if (key == GLFW_KEY_D && action == GLFW_PRESS)
    //     {
    //         // // move the player, reconstitute camera?
    //         // auto [pos, vel] = registry.get<position, velocity>(player);
    //         // pos += camera.right * vel; 
    //     }
    // }

    // //@IC(Sjors): parameters cannot be const since the callbacks needs to match.
    // //@Dependencies: camera.
    // void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    // {
    //     // double delta_x = xpos - last_x;
    //     // double delta_y = last_y - ypos;     
    //     // // update
    //     // last_x = xpos;
    //     // last_y = ypos;

    //     // update_camera_with_mouse_input(camera, delta_x, delta_y);

    // }

    // // dependencies: gl call
    // void glfw_window_size_callback(GLFWwindow* window, int width, int height)
    // {
    //     glViewport(0, 0, width, height);
    // }

    // //dependencies: holding_left_mouse_button, holding_right_mouse_button
    // void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    // {
    //     if (button == GLFW_MOUSE_BUTTON_LEFT)
    //     {
    //         if (action == GLFW_PRESS)
    //         {
    //             // holding_left_mouse_button = true;
    //         }
    //         else if (action == GLFW_RELEASE)
    //         {
    //             // holding_left_mouse_button = false;
    //         }
    //     }
    //     if (button == GLFW_MOUSE_BUTTON_RIGHT)
    //     {
    //         if (action == GLFW_PRESS)
    //         {
    //             // holding_right_mouse_button = true;
    //         }
    //         else if (action == GLFW_RELEASE)
    //         {
    //             // holding_right_mouse_button = false;
    //         }
    //     }
    // }

    // //@dependencies:
    // // scroll_delta_y
    // void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    // {
    //     // scroll_delta_y = yoffset;
    // }
}


//@Dependencies:
// window
// window_height,
// window_width,
// vsync_enabled?
// all of the callbacks!
GLFWwindow* glfw_init_window(const char* title, const int window_width, const int window_height)
{
    ///--- init glfw ----
    if (!glfwInit())
    {
        logr::report_error("glfw cannot init!");
        exit(1);        
    }
    else 
    {
        logr::report("glfw inited.\n");
    }
    // what version of openGL do we want?
    // const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, title, nullptr, nullptr);
    if (nullptr == window)
    {
        logr::report_error("window is nullptr");
        exit(1);
    }

    glfwSetWindowPos(window, 20, 20);
    //@IMPORTANT!
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // capture cursor & disable it.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    bool error = (gladLoadGL() == 0);
    if (error)
    {
        logr::report_error("Failed to initialize OpenGL loader!\n");
        exit(1);
    }


    return window;
}

GLFWwindow* glfw_init_debug_window(const int debug_window_width, const int debug_window_height, GLFWwindow* parent_window)
{   
    // focus cursor on original window
    glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
    GLFWwindow* debug_window = glfwCreateWindow(850, 1000, "Debug", NULL, parent_window);
    if (nullptr == debug_window)
    {
        logr::report_error("debug_window is nullptr");
        exit(1);
    }
    glfwMakeContextCurrent(debug_window);
    glfwSetWindowPos(debug_window, 1120, 100);
    glfwSwapInterval(1); // Enable vsync

    ///--- init IMGUI ----
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    const char* glsl_version = "#version 450";
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(debug_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    

    return debug_window;
}

//@dependencies:
// window
// debug_window
// debug_window_width
// debug_window_height
void glfw_exit_and_cleanup(GLFWwindow* window)
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}