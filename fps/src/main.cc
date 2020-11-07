
#include "window_manager.h"
#include "shader_manager.h"
#include "texture_manager.h"

#include "render_system.h"
#include "task_system.h"

#include "log.h"
#include "particle.h"
#include "game.h"

#include <chrono>
#include <thread>


const int window_width = 1920;
const int window_height = 1080;
const int debug_window_width = 850;
const int debug_window_height = 1000;

int main()
{
    //@TODO(Sjors): why does using two task systems slow down the application to a crawl?
    //@TODO(Sjors): how to deal with openGL context w. multiple threads?
    auto high_priority_queue = Task_System(3); 
    // auto low_priority_queue =  Task_System(1);


    auto window_manager = Window_Manager();
    create_main_window(window_manager, "mvmt", window_width, window_height);

    auto shader_manager = Shader_Manager();
    load_shader(shader_manager, "deferred_geometry");
    load_shader(shader_manager, "deferred_lighting");
    load_shader(shader_manager, "deferred_obra_dinn");
    load_shader(shader_manager, "deferred_edges");
    load_shader(shader_manager, "deferred_normals");
    load_shader(shader_manager, "lightbox");
    load_shader(shader_manager, "simple_depth");




    using namespace std::chrono_literals;
    std::this_thread::sleep_for(8s);

    auto texture_manager = Texture_Manager();
    load_png_texture(texture_manager, "metal");
    load_png_texture(texture_manager, "marble");

    //@Fixme(Sjors): create a render manager?
    init_renderer(shader_manager, texture_manager, window_width, window_height);

    Game_State game_state{};
    Particle_Cache particle_cache{}; // uh, allocating this on the stack is maybe a bad idea.]

    game_state.game_mode = GM_GAME;

    // Main loop
    float frame_dt = 0.0f;
    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::time_point<std::chrono::system_clock> end;
    double average_frame_time[120] = {0};
    uint32_t current_idx = 0;

    while (true)
    {
        start = std::chrono::system_clock::now();
        poll_input(window_manager);
        game_simulate(frame_dt, game_state, window_manager.input, particle_cache);
        game_render(game_state, particle_cache);

        // if (game_state.game_mode == GM_EDITOR) render_debug_ui(window_manager);

        // swap buffers is done before delta time, is that even correct?
        swap_buffers(window_manager);

        end = std::chrono::system_clock::now();
        double delta = static_cast<std::chrono::duration<double>>(end - start).count();
        average_frame_time[current_idx % 120] = delta;
        current_idx += 1;
        
        if (current_idx % 120 == 0) 
        {
            double accumulator = 0.0;
            for (auto& value: average_frame_time)
                accumulator += value;
            accumulator /= 120.0;
            std::cout << "average frame time past 120 frames: " << accumulator << '\n';
        }

    }

    //@todo: delete glfw?
    logr::report("exiting...");

    return 0;
}
