
#include "window_manager.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "asset_manager.h"
#include "entity_manager.h"

#include "render_system.h"
#include "sound_system.h"
#include "task_system.h"

#include "log.h"
#include "particle.h"
#include "game.h"

#include "timed_function.h"

#include <chrono>
#include <thread>
#include <array>
#include <numeric> // for accumulate

const int window_width = 1920;
const int window_height = 1080;
const int debug_window_width = 850;
const int debug_window_height = 1000;

int main()
{
    //@TODO(Sjors): why does using two task systems slow down the application to a crawl?
    //@TODO(Sjors): how to deal with openGL context w. multiple threads?
    constexpr const int worker_count{3};
    // auto high_priority_queue = Task_System(worker_count); 

    auto window_manager = Window_Manager();
    create_main_window(window_manager, "fps", window_width, window_height);

    auto shader_manager = Shader_Manager();
    load_shader(shader_manager, "deferred_geometry");
    load_shader(shader_manager, "deferred_instanced");
    load_shader(shader_manager, "deferred_lighting");
    load_shader(shader_manager, "lightbox");
    load_shader(shader_manager, "debug_geometry");
    load_shader(shader_manager, "simple_depth");
    load_shader(shader_manager, "simple_depth_shadow_mapping");
    load_shader(shader_manager, "screen_space");

    auto asset_manager = Asset_Manager();
    bool should_unitize = true;
    load_obj(asset_manager, "new_spear", should_unitize);
    load_obj(asset_manager, "dodecahedron");

    auto texture_manager = Texture_Manager();
    load_png_texture(texture_manager, "metal");
    load_png_texture(texture_manager, "marble");
    load_tga_texture(texture_manager, "target_wood_diffuse");
    load_png_texture(texture_manager, "wall_64");
    load_png_texture(texture_manager, "floor_64");
    load_png_texture(texture_manager, "star_64");
    load_png_texture(texture_manager, "wall_stone_specular");
    load_png_texture(texture_manager, "wall_stone_diffuse");
    load_png_texture(texture_manager, "wall_stone_normal");
    load_png_texture(texture_manager, "new_spear_diffuse");
    load_png_texture(texture_manager, "dodecahedron");
    load_alpha_png_texture(texture_manager, "crosshair");

    auto entity_manager = Entity_Manager();
    for (size_t idx = 0; idx != 100; ++idx)
        create_entity(entity_manager, Entity_Type::Cube);


    init_sound_system();
    load_sound("chicken.wav");
    load_sound("plop.wav");

    //@Fixme(Sjors): create a render manager?
    init_renderer(shader_manager, texture_manager, asset_manager, entity_manager, window_width, window_height);

    Game_State game_state{};
    Particle_Cache particle_cache{}; // uh, allocating this on the stack is maybe a bad idea.

    game_state.game_mode = GM_GAME;

    // Main loop
    float frame_dt = 0.0f;
    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::time_point<std::chrono::system_clock> end;
    constexpr const int buffer_size = 256;
    std::array<double, buffer_size> frame_time_ring_buffer{};
    std::array<double, buffer_size> render_time_ring_buffer{}; 

    uint32_t current_idx = 0;

    while (true)
    {
        start = std::chrono::system_clock::now();
        poll_input(window_manager);
        game_simulate(frame_dt, game_state, window_manager.input, particle_cache);
        game_render(game_state, particle_cache);

        if (game_state.game_mode == GM_EDITOR) render_debug_ui(window_manager);

        swap_buffers(window_manager);

        // some time calculation.
        end = std::chrono::system_clock::now();
        const double delta = static_cast<std::chrono::duration<double>>(end - start).count();
        const auto buffer_idx = current_idx % buffer_size;
        frame_time_ring_buffer[buffer_idx] = delta;
        render_time_ring_buffer[buffer_idx] = Timed_Function::get_duration("render");
        current_idx += 1;
    
        if (current_idx % buffer_size == 0) 
        {
            const double frame_average =  std::accumulate(frame_time_ring_buffer.begin(), frame_time_ring_buffer.end(), 0.0) / static_cast<double>(buffer_size);
            const double render_average = std::accumulate(render_time_ring_buffer.begin(), render_time_ring_buffer.end(), 0.0) / static_cast<double>(buffer_size);

            Timed_Function::timed_functions["fps"].duration = 1.0 / frame_average;
            Timed_Function::timed_functions["average_frame_time"].duration = frame_average;
            Timed_Function::timed_functions["render_time"].duration = render_average;
        }
    }

    //@todo: delete glfw?
    logr::report("exiting...");

    return 0;
}

