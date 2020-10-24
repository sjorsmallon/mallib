#include "window_manager.h"
#include "render_system.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "log.h"

// #define     GLFW_KEY_W   87
constexpr const int KEY_W = 87;

int main()
{
    const int window_width = 1920;
    const int window_height = 1080;
    const int debug_window_width = 850;
    const int debug_window_height = 1000;

    // GLFW abstraction
    auto window_manager = Window_Manager();
    create_main_window(window_manager, "mvmt", window_width, window_height);

    auto input_manager = Input_Manager();
    register_command(input_manager,  KEY_W, std::move([](){
        logr::report("test.\n");
    }));

    auto shader_manager = Shader_Manager();
    load_shader(shader_manager, "deferred_geometry");
    load_shader(shader_manager, "deferred_lighting");
    load_shader(shader_manager, "lightbox");
    load_shader(shader_manager, "simple_depth");

    auto texture_manager = Texture_Manager();
    load_png_texture(texture_manager, "metal");
    load_png_texture(texture_manager, "marble");

    init_renderer(shader_manager, texture_manager, window_width, window_height);




    float frame_dt = 0.0f;
    float last_frame_time = 0.0f;

    // Main loop
    while (true)
    {
        //     float current_frame_time = glfwGetTime();
        //     frame_dt = current_frame_time - last_frame_time;
        //     last_frame_time = current_frame_time;


        poll_input(window_manager);
        // simulate();
        render();
        swap_buffers(window_manager);

    }

    //@todo: delete glfw.
    logr::report("exiting...");

    return 0;
}


//@Note(Sjors:)
//  At this point, no resources have been allocated or loaded. But these 
//  all require GL to be loaded in. creating a window does this for us via
// gladLoadGL in the window_manager constructor. Which seems like a bad idea.
// We need to make sure GL is ready before we can test these.
 




   //     // render_ui();

    //     // bool show_ui = true;
    //     // if (show_ui)
    //     // {
    //     //     glfwMakeContextCurrent(imgui_window);
    //     //     // Start the Dear ImGui frame
    //     //     ImGui_ImplOpenGL3_NewFrame();
    //     //     ImGui_ImplGlfw_NewFrame();
    //     //     ImGui::NewFrame();

    //     //     {
    //     //         ImGui::Begin("Active Model");                      
    //     //         ImGui::End();

    //     //         ImGui::Begin("Active Camera");
    //     //         ImGui::End();
    //     //     }

    //     //     ImGui::Render();
    //     //     int display_w, display_h;
    //     //     glfwGetFramebufferSize(window, &display_w, &display_h);
    //     //     glViewport(0, 0, display_w, display_h);

    //     //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //     //     glfwSwapBuffers(imgui_window);
    //     //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     // }
    // }