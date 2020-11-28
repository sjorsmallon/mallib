
#include "window_manager.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "asset_manager.h"
#include "entity_manager.h"

#include "render_system.h"
#include "sound_system.h"
#include "task_system.h"

#include "logr.h"
#include "particle.h"
#include "game.h"

#include "timed_function.h"

#include <chrono>
#include <thread>
#include <array>

const int window_width = 1920;
const int window_height = 1080;
const int debug_window_width = 850;
const int debug_window_height = 1000;

int main()
{
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
    load_obj(asset_manager, "arrow");


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
    load_png_texture(texture_manager, "arrow_diffuse");
    load_png_texture(texture_manager, "angry_face");
    load_alpha_png_texture(texture_manager, "crosshair");

    auto entity_manager = Entity_Manager();
    for (size_t idx = 0; idx != 255; ++idx)
        create_entity(entity_manager, Entity_Type::Cube);
    make_entities_active(entity_manager);

    init_sound_system();
    load_sound("chicken.wav");
    load_sound("plop.wav");
    load_sound("plop_shorter_runup.wav");

    //@Fixme(Sjors): create a render manager?
    init_renderer(
        shader_manager,
        texture_manager,
        asset_manager,
        entity_manager,
        window_width,
        window_height);

    Game_State game_state{};
    game_state.game_mode = GM_GAME;
    game_init();
    
    Particle_Cache particle_cache{}; // uh, allocating this on the stack is maybe a bad idea.

    // Main loop
    Frame_Timer frame_timer{};
    double frame_dt = 0.0f;
    uint32_t frame_count = 0;
    while (true)
    {
        start(frame_timer);

        poll_input(window_manager);
        game_simulate(game_state, frame_dt, window_manager.input, particle_cache, entity_manager);
        game_render(game_state, particle_cache);
        game_end_frame(game_state, entity_manager);

        if (game_state.game_mode == GM_EDITOR) render_debug_ui(window_manager);

        swap_buffers(window_manager);

        stop(frame_timer);
        maybe_log_average(frame_timer, frame_count);

        frame_dt = frame_timer.dt;
        frame_count +=1;
    }

    //@todo: delete glfw?
    logr::report("exiting...");

    return 0;
}

