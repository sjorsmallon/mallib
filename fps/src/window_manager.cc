#include "window_manager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <log/log.h>

namespace
{

}


//@Dependencies:
// window
// window_height,
// window_width,
// vsync_enabled?
// opengl version
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
    // glfwSwapInterval(1); // Enable vsync

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