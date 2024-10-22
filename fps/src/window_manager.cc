#include "window_manager.h"

#include "input.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "logr.h"
#include "timed_function.h"

namespace
{
   void glfw_close(GLFWwindow* window)
    {
        logr::report("[GLFW] closing down the window (calling exit).\n");
        exit(1);
    }

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
        if (key == GLFW_KEY_M) glfwSetInputMode(window_manager->main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
        if (key == GLFW_KEY_N) glfwSetInputMode(window_manager->main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
        if (key == GLFW_KEY_V) glfwSwapInterval(1); // vsync
        if (key == GLFW_KEY_U) glfwSwapInterval(0); // unlocked



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
    // input
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
    // input->scroll_delta_y 
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
    // Request particular opengl version (4.5)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


    // Create window with graphics context
    GLFWwindow* main_window = glfwCreateWindow(window_width, window_height, title, nullptr, nullptr);
    assert(nullptr != main_window);

    glfwSetWindowPos(main_window, 20, 20);
    //@IMPORTANT!
    glfwMakeContextCurrent(main_window);

    //@IC(Sjors): gladLoadGL only after makeContextCurrent.    
    bool error = (gladLoadGL() == 0);
    if (error)
    {
        logr::report_error("Failed to initialize OpenGL loader!\n");
        exit(1);
    }

    glfwSwapInterval(1); // Enable vsync
    //  glfwSwapInterval(0); // explicitly disable vsync?

    // register callbacks
    // glfwSetErrorCallback(glfw_error_callback); (this is done in the constructor since it does not require a window.)
    glfwSetKeyCallback(main_window, glfw_key_callback);
    glfwSetMouseButtonCallback(main_window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(main_window, glfw_cursor_position_callback);
    glfwSetScrollCallback(main_window, glfw_scroll_callback);
    
    // set user pointer to this window so we can access input in the glfw callbacks.
    glfwSetWindowUserPointer(main_window, reinterpret_cast<void*>(&window_manager));

    ///--- init IMGUI ----
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    const char* glsl_version = "#version 450";
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(main_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    window_manager.main_window = main_window;

    //hide the cursor, unhide if in debug mode
    glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
}

//@TODO(Sjors): swap, clear, poll.
void poll_input(Window_Manager& window_manager)
{
    //@TODO(Sjors): why do we want to do this exactly? at this moment, 
    // the keyboard perfectly reflects which keys are being pressed. only GLFW_RELEASE
    // can toggle things back to false.
    // We could argue that if the event has already been processed (since the event would be repeated)
    // that we could continue to do something else.
    // The point being that I don't know how to deal with individual keypresses (what if we want to deal with being held?)
    // swap the pointers of current keyboard state and previous keyboard state.
    // std::swap(window_manager.input.keyboard_state,window_manageþr.input.prev_keyboard_state);

    // swap mouse buttons
    window_manager.input.prev_mouse_left = window_manager.input.mouse_left; 
    window_manager.input.prev_mouse_right = window_manager.input.mouse_right;

    // set both to false
    window_manager.input.mouse_left = false;
    window_manager.input.mouse_right = false;

    // set mouse delta to zero.
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

//@dependencies:
// -  main_window pointer
// - logr::console_log
// void render_debug_ui(const Window_Manager& window_manager)
// {
    // unhide the cursor.
    // glfwSetInputMode(window_manager.main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
    // {
    //     ImGui_ImplOpenGL3_NewFrame();
    //     ImGui_ImplGlfw_NewFrame();
    //     ImGui::NewFrame();

    //     // Debug Menu
    //     {
    //         ImGui::Begin("General Settings");
    //         if (ImGui::Button("Vsync on")) glfwSwapInterval(1);
    //         if (ImGui::Button("Vsync off")) glfwSwapInterval(0);
    //         ImGui::End();

    //         {
    //             ImGui::Begin("Timed Functions");
    //             for (auto& [key, value]: Timed_Function::timed_functions)
    //             ImGui::Text("%s : %f", key.c_str(), value.duration);
    //             ImGui::End();
    //         }

    //         {
    //             ImGui::Begin("Debug Menu"); 
    //             for (auto& debug_variables: logr::debug_variables())
    //                 ImGui::InputFloat(debug_variables.name, debug_variables.value, debug_variables.min, debug_variables.max, "%.3f");

    //             ImGui::End();
    //         }
            

    //     }

    //     ImGui::EndFrame();
    //     ImGui::Render();
    //     int display_w, display_h;
    //     glfwGetFramebufferSize(window_manager.main_window, &display_w, &display_h);
    //     glViewport(0, 0, display_w, display_h);

    //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // }
    // glfwSetInputMode(window_manager.main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
// }


