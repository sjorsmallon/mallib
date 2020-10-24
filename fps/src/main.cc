#include "window_manager.h"
#include "log.h"
#include "render_system.h"
#include "shader_manager.h"
#include "texture_manager.h"



int main()
{
    // window - settings
    const int window_width = 1920;
    const int window_height = 1080;
    const int debug_window_width = 850;
    const int debug_window_height = 1000;

    auto window_manager = Window_Manager();
    create_main_window(window_manager, "mvmt", window_width, window_height);
    // create_debug_window(window_manager, "debug", debug_window_width, debug_window_height);


    // at this point, no resources have been allocated or loaded. 



    Shader_Manager shader_manager{};
    load_shader(shader_manager, "deferred_geometry");
    load_shader(shader_manager, "deferred_lighting");
    load_shader(shader_manager, "lightbox");
    load_shader(shader_manager, "simple_depth");

    Texture_Manager texture_manager{};
    load_png_texture(texture_manager, "metal");

    init_renderer(shader_manager, texture_manager, window_width, window_height);

    // Entity_Manager entity_manager{};
    // create_player(entity_manager);
    // create_lights(entity_manager);


    float frame_dt = 0.0f;
    float last_frame_time = 0.0f;

    // Main loop
    while (true)
    {
        poll_input(window_manager);
        // simulate();

        render();
        swap_buffers(window_manager);

    }
    // while (!glfwWindowShouldClose(main_window))
    // {
    //     // per-frame time logic
    //     // --------------------
    //     float current_frame_time = glfwGetTime();
    //     frame_dt = current_frame_time - last_frame_time;
    //     last_frame_time = current_frame_time;
      
    //     // Poll and handle events (inputs, window resize, etc.)
    //     // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    //     // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    //     // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    //     // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    //     glfwPollEvents();
    //     if (KEY_W_DOWN) update_player_position_with_key_input(KEY_W);
    //     if (KEY_S_DOWN) update_player_position_with_key_input(KEY_S);
    //     if (KEY_A_DOWN) update_player_position_with_key_input(KEY_A);
    //     if (KEY_D_DOWN) update_player_position_with_key_input(KEY_D);

    //     render();
    //     glfwSwapBuffers(main_window);
    //     // render();

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

    //@todo: delete glfw.
    logr::report("exiting...");

    return 0;
}
 


