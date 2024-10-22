
#include <chrono>

#include "mallet.h"
#include "plane.h"

#include "window_manager.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "asset_manager.h"
#include "entity_manager.h"

#include "render_system.h"
#include "sound_system.h"

#include "file_watcher.h"

#include "logr.h"
#include "particle.h"
#include "game.h"

#include "timed_function.h"

#include <chrono>
#include <thread>
#include <array>

using namespace std::literals::chrono_literals;

const int window_width = 1920;
const int window_height = 1080;

int main()
{

    load_map_from_file("../assets/maps/AABB.map");

    auto window_manager = Window_Manager();
    //@FIXME(SMIA): create_main_window does not make sense, we can just create a window and say
    // whether it should be in focus or not.
    create_main_window(window_manager, "fps", window_width, window_height);

    auto shader_manager = Shader_Manager();
    set_shader_path(shader_manager, "../assets/shaders/");

    load_shader(shader_manager, "deferred_pbr");
    load_shader(shader_manager, "deferred_instanced_pbr");
    load_shader(shader_manager, "deferred_instanced_pbr_no_mvp");
    load_shader(shader_manager, "post_deferred_pbr");
    load_shader(shader_manager, "screen_space");
    load_shader(shader_manager, "debug_position_world");

    //@Fixme(Sjors): this is an ugly hack to enable the file_watcher lambda
    // to access the shader manager.
    set_global_shader_manager(shader_manager);

    auto file_watcher = File_Watcher();
    watch_folder(file_watcher, "../assets/shaders/post_deferred_parallax_pbr", 100ms, shader_reload_callback);

    auto asset_manager = Asset_Manager();
    set_asset_path(asset_manager, "../assets/obj/");
    bool should_unitize = true;

    load_obj(asset_manager, "sphere", should_unitize);
    load_obj(asset_manager, "new_spear", should_unitize);
    load_obj(asset_manager, "cube", should_unitize);
    load_obj(asset_manager, "dodecahedron");
    load_obj(asset_manager, "arrow");

    auto texture_manager = Texture_Manager();
    set_texture_path(texture_manager, "../assets/textures/");
    load_png_texture(texture_manager, "painted_metal/painted_metal_2K_color");
    load_png_texture(texture_manager, "painted_metal/painted_metal_2K_roughness");
    load_png_texture(texture_manager, "painted_metal/painted_metal_2K_normal");
    load_png_texture(texture_manager, "painted_metal/painted_metal_2K_displacement");
    load_png_texture(texture_manager, "painted_metal/painted_metal_2K_metallic");
    load_png_texture(texture_manager, "painted_metal/painted_metal_2K_ambient_occlusion");

    load_png_texture(texture_manager, "moss/moss_2K_color");
    load_png_texture(texture_manager, "moss/moss_2K_roughness");
    load_png_texture(texture_manager, "moss/moss_2K_normal");
    load_png_texture(texture_manager, "moss/moss_2K_displacement");
    load_png_texture(texture_manager, "moss/moss_2K_ambient_occlusion");

    // what is the difference here? how do we know which textures are transparent upon drawing?
    load_alpha_png_texture(texture_manager, "crosshair");

    auto entity_manager = Entity_Manager();

    init_sound_system();
    set_sound_path("../assets/sounds/");
    load_sound("chicken.wav");
    load_sound("plop.wav");
    load_sound("plop_shorter_runup.wav");
    load_sound("applause.ogg");
    load_sound("weapons/rocket_launcher/dk_barrel_blast.ogg");
    load_sound("weapons/bananade/grenade_thoomp.wav");

    load_sound("finger_snap.wav");

    //@Fixme(Sjors): create a render manager?
    init_render_system(
        shader_manager,
        texture_manager,
        asset_manager,
        entity_manager,
        window_width,
        window_height);

    Game_State game_state{};
    game_state.game_mode = GM_GAME;
    game_init(entity_manager);
    
    Particle_Cache particle_cache{}; // uh, allocating this on the stack is maybe a bad idea.

    // Main loop
    Timer frame_timer{};
    double frame_dt = 0.0f;
    uint32_t frame_count = 0;
    while (true)
    {
        timer_start(frame_timer);

        poll_input(window_manager);
        game_simulate(game_state, frame_dt, window_manager.input, particle_cache, entity_manager);
    
        draw_AABBs();

        game_render(game_state, particle_cache);
        game_end_frame(game_state, entity_manager);

        if (game_state.game_mode == GM_EDITOR) render_debug_ui();

        swap_buffers(window_manager);
        timer_stop(frame_timer);
        
        timer_maybe_log_average(frame_timer, frame_count);
        maybe_watch_files(file_watcher, frame_timer.start);

        frame_dt = frame_timer.dt;
        frame_count += 1;
    }

    //@todo: delete glfw?
    logr::report("exiting...");

    return 0;
}

