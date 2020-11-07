#include "window_manager.h"

#include "input.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "log.h"

namespace
{
    // bool holding_left_mouse_button = false;
    // bool holding_right_mouse_button = false;
    // // DUMB input handling.
    // bool KEY_W_DOWN = false;
    // bool KEY_S_DOWN = false;
    // bool KEY_A_DOWN = false;
    // bool KEY_D_DOWN = false;


   void glfw_close(GLFWwindow* window)
    {
        logr::report("[GLFW] closing down the window (calling exit).\n");
        exit(1);
    }

    //@IC(Sjors): parameters cannot be const since the callbacks needs to match.
    void glfw_error_callback(int error, const char* description)
    {
        logr::report_error("[GLFW]: {}, {} \n", error, description);
    }

    
    // the action is either GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE.    
    void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Window_Manager* window_manager = reinterpret_cast<Window_Manager*>(glfwGetWindowUserPointer(window));
        auto& input = window_manager->input;

        // uh...
        if (key == GLFW_KEY_ESCAPE) glfw_close(window);

        // otherwise
        input.keyboard_state[key] = (action == GLFW_PRESS || action == GLFW_REPEAT) ? true : false; 

    }

    //@IC(Sjors): parameters cannot be const since the callbacks needs to match.
    void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Window_Manager* window_manager = reinterpret_cast<Window_Manager*>(glfwGetWindowUserPointer(window));
        auto& input = window_manager->input;

        input.mouse_delta_x = xpos - input.mouse_x;
        input.mouse_delta_y = input.mouse_y - ypos;     

        // update
        input.mouse_x = xpos;
        input.mouse_y = ypos;

    }

    //@Dependencies:[GL]
    void glfw_window_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    //@Dependencies:
    // holding_left_mouse_button,
    // holding_right_mouse_button
    void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        Window_Manager* window_manager = reinterpret_cast<Window_Manager*>(glfwGetWindowUserPointer(window));
        auto& input = window_manager->input;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                input.mouse_left = true;
            }
            else if (action == GLFW_RELEASE)
            {
                input.mouse_left = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                input.mouse_right = true;
            }
            else if (action == GLFW_RELEASE)
            {
                input.mouse_right = false;
            }
        }
        }

    //@dependencies:
    // scroll_delta_y
    void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        Window_Manager* window_manager = reinterpret_cast<Window_Manager*>(glfwGetWindowUserPointer(window));
        auto& input = window_manager->input;

        // static double scroll_delta_y = yoffset;
        // scroll_delta_y = yoffset;
    }

}


Window_Manager::Window_Manager()
{
     ///--- init glfw ----
    if (!glfwInit())
    {
        logr::report_error("[Window_Manager] glfw cannot init!");
        exit(1);        
    }
    else 
    {
        logr::report("[Window_Manager] glfw inited.\n");
    }

    glfwSetErrorCallback(glfw_error_callback);

}

Window_Manager::~Window_Manager()
{
    // Cleanup Debug (ImGui Window)
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(main_window);
    glfwDestroyWindow(debug_window);

    glfwTerminate();
}


void create_main_window(Window_Manager& window_manager, const char* title, const int window_width, const int window_height)
{
     // what version of openGL do we want?
    // const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    // Create window with graphics context

    GLFWwindow* main_window = glfwCreateWindow(window_width, window_height, title, nullptr, nullptr);
    if (nullptr == main_window)
    {
        logr::report_error("window is nullptr");
        exit(1);
    }

    glfwSetWindowPos(main_window, 20, 20);
    //@IMPORTANT!
    glfwMakeContextCurrent(main_window);
    // glfwSwapInterval(1); // Enable vsync
    glfwSwapInterval( 0 ); // explicitly disable vsync?

    //@Note(Sjors): gladLoadGL only after makeContextCurrent.    
    bool error = (gladLoadGL() == 0);
    if (error)
    {
        logr::report_error("Failed to initialize OpenGL loader!\n");
        exit(1);
    }

    //@FIXME: disable cursor when we are trying to access the debug menu?
    // capture cursor & disable it.

    // register callbacks
    // glfwSetErrorCallback(glfw_error_callback); (this is done in the constructor since it does not require a window.)
    glfwSetKeyCallback(main_window, glfw_key_callback);
    glfwSetMouseButtonCallback(main_window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(main_window, glfw_cursor_position_callback);
    glfwSetScrollCallback(main_window, glfw_scroll_callback);
    
    // set user pointer
    glfwSetWindowUserPointer(main_window, reinterpret_cast<void*>(&window_manager));


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
    ImGui_ImplGlfw_InitForOpenGL(main_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    window_manager.main_window = main_window;

    //hide the cursor, unhide if in debug mode
    glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 



}

void create_debug_window(Window_Manager& window_manager, const char* title, const int window_width, const int window_height)
{
    // the debug window has a parent window. That is the main window. 
    // This is implictly reached by the window manager.
    GLFWwindow* parent_window = window_manager.main_window;


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
    // glfwSwapInterval(1); // Enable vsync

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
    
    window_manager.debug_window = debug_window;


    glfwMakeContextCurrent(parent_window);

//     return debug_window;
}


// swap, clear & poll
void poll_input(Window_Manager& window_manager)
{
    window_manager.input.mouse_delta_x =  0.0f;
    window_manager.input.mouse_delta_y =  0.0f;

    glfwPollEvents();
}

void swap_buffers(const Window_Manager& window_manager)
{
    glfwMakeContextCurrent(window_manager.main_window);
    glfwSwapBuffers(window_manager.main_window);

    if (!(nullptr == window_manager.debug_window)) 
    {

        glfwMakeContextCurrent(window_manager.debug_window);
        glfwSwapBuffers(window_manager.main_window);
        glfwMakeContextCurrent(window_manager.main_window);
    }
}


//@dependencies: main_window pointer
void render_debug_ui(const Window_Manager& window_manager)
{
    // unhide the cursor.
    glfwSetInputMode(window_manager.main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 

    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Debug Menu
        {
            ImGui::Begin("Debug Menu");                      
            ImGui::End();
        }

        ImGui::EndFrame();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_manager.main_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}



//@Note(Sjors): Keep these snippets around for a quick start to init glfw and create a window.

// GLFWwindow* glfw_init_window(const char* title, const int window_width, const int window_height)
// {
//     ///--- init glfw ----
//     if (!glfwInit())
//     {
//         logr::report_error("glfw cannot init!");
//         exit(1);        
//     }
//     else 
//     {
//         logr::report("glfw inited.\n");
//     }

//     // what version of openGL do we want?
//     // const char* glsl_version = "#version 430";
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
//     // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
//     glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
//     // Create window with graphics context

//     GLFWwindow* window = glfwCreateWindow(window_width, window_height, title, nullptr, nullptr);
//     if (nullptr == window)
//     {
//         logr::report_error("window is nullptr");
//         exit(1);
//     }

//     glfwSetWindowPos(window, 20, 20);
//     //@IMPORTANT!
//     glfwMakeContextCurrent(window);
//     // glfwSwapInterval(1); // Enable vsync

//     //@Note(Sjors): gladLoadGL only after makeContextCurrent.    
//     bool error = (gladLoadGL() == 0);
//     if (error)
//     {
//         logr::report_error("Failed to initialize OpenGL loader!\n");
//         exit(1);
//     }

//     // capture cursor & disable it.
//     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 


//     return window;
// }

// GLFWwindow* glfw_init_debug_window(const int debug_window_width, const int debug_window_height, GLFWwindow* parent_window)
// {   
//     // focus cursor on original window
//     glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
//     GLFWwindow* debug_window = glfwCreateWindow(850, 1000, "Debug", NULL, parent_window);
//     if (nullptr == debug_window)
//     {
//         logr::report_error("debug_window is nullptr");
//         exit(1);
//     }
//     glfwMakeContextCurrent(debug_window);
//     glfwSetWindowPos(debug_window, 1120, 100);
//     glfwSwapInterval(1); // Enable vsync

//     ///--- init IMGUI ----
//     // Setup Dear ImGui context
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();

//     ImGuiIO& io = ImGui::GetIO(); (void)io;
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//     // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


//     const char* glsl_version = "#version 450";
//     // Setup Dear ImGui style
//     ImGui::StyleColorsDark();
//     // Setup Platform/Renderer bindings
//     ImGui_ImplGlfw_InitForOpenGL(debug_window, true);
//     ImGui_ImplOpenGL3_Init(glsl_version);
    

//     return debug_window;
// }

