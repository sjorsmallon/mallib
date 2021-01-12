
#include <chrono>

#include "window_manager.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "asset_manager.h"
#include "entity_manager.h"

#include "render_system.h"
#include "sound_system.h"
#include "task_system.h"

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
const int debug_window_width = 850;
const int debug_window_height = 1000;

int main()
{
    auto window_manager = Window_Manager();
    create_main_window(window_manager, "fps", window_width, window_height);

    auto shader_manager = Shader_Manager();
    set_shader_path(shader_manager, "../assets/shaders/");

    load_shader(shader_manager, "deferred_pbr");
    load_shader(shader_manager, "deferred_instanced_pbr");
    load_shader(shader_manager, "deferred_parallax_pbr");
    load_shader(shader_manager, "post_deferred_parallax_pbr");
    load_shader(shader_manager, "post_deferred_pbr");

    // load_shader(shader_manager, "deferred_geometry");
    // load_shader(shader_manager, "deferred_instanced");
    // load_shader(shader_manager, "post_deferred_lighting");

    // load_shader(shader_manager, "lightbox");
    // load_shader(shader_manager, "debug_geometry");
    
    // load_shader(shader_manager, "simple_depth");
    // load_shader(shader_manager, "simple_depth_shadow_mapping");
    // load_shader(shader_manager, "screen_space");


    //@Fixme(Sjors): this is an ugly hack to enable the file_watcher lambda
    // to access the shader manager.
    set_global_shader_manager(shader_manager);

    auto file_watcher = File_Watcher();
    watch_folder(file_watcher, "../assets/shaders/post_deferred_parallax_pbr", 100ms, [](const std::string& shader_folder_name){
        // get the shader manager.
        Shader_Manager& manager = get_global_shader_manager();
        std::string shader_name = shader_folder_name.substr(shader_folder_name.find_last_of('/') + 1);
        logr::report("shader_name: {}\n", shader_name);
        bool shader_found = manager.shaders.find(shader_name) != manager.shaders.end();
        
        assert(shader_found && "shader not found!");

        if (shader_found)
        {
            // clear shader info, except for name.
            clear_shader_gl_components(manager, shader_name);
            // at this point, the shader no longer exists, and we can (re)load the shader.
            load_shader(manager, shader_name);
        }
    });


    auto asset_manager = Asset_Manager();
    set_asset_path(asset_manager, "../assets/obj/");
    bool should_unitize = true;

    load_obj(asset_manager, "sphere", should_unitize);
    load_obj(asset_manager, "new_spear", should_unitize);
    load_obj(asset_manager, "cube", should_unitize);
    load_obj(asset_manager, "dodecahedron");
    load_obj(asset_manager, "chicken");
    load_obj(asset_manager, "arrow");

    auto texture_manager = Texture_Manager();
    set_texture_path(texture_manager, "../assets/textures/");
    
    // load_png_texture(texture_manager, "metal");
    // load_png_texture(texture_manager, "marble");
    // load_png_texture(texture_manager, "wall_64");
    // load_png_texture(texture_manager, "floor_64");
    // load_png_texture(texture_manager, "star_64");
    // load_png_texture(texture_manager, "wall_stone_specular");
    // load_png_texture(texture_manager, "wall_stone_diffuse");
    // load_png_texture(texture_manager, "wall_stone_normal");
    load_png_texture(texture_manager, "pavingstones_4K_color");
    load_png_texture(texture_manager, "pavingstones_4K_normal");
    load_png_texture(texture_manager, "pavingstones_4K_roughness");
    load_png_texture(texture_manager, "pavingstones_4K_displacement");
    load_png_texture(texture_manager, "pavingstones_4K_ambient_occlusion");
    // load_png_texture(texture_manager, "new_spear_diffuse");
    // load_png_texture(texture_manager, "dodecahedron");
    // load_png_texture(texture_manager, "arrow_diffuse");
    // load_png_texture(texture_manager, "angry_face");
    // load_png_texture(texture_manager, "concrete_4K_color");
    // load_png_texture(texture_manager, "concrete_4K_ambient_occlusion");
    // load_png_texture(texture_manager, "concrete_4K_roughness");
    // load_png_texture(texture_manager, "concrete_4K_normal");
    // load_png_texture(texture_manager, "concrete_4K_displacement");



    load_alpha_png_texture(texture_manager, "crosshair");
    load_tga_texture(texture_manager, "target_wood_diffuse");


    auto entity_manager = Entity_Manager();
    for (size_t idx = 0; idx != 10; ++idx) create_entity(entity_manager, Entity_Type::Cube);

    init_sound_system();

    load_sound("chicken.wav");
    load_sound("plop.wav");
    load_sound("plop_shorter_runup.wav");
    load_sound("applause.ogg");
    load_sound("finger_snap.wav");

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
    Timer frame_timer{};
    double frame_dt = 0.0f;
    uint32_t frame_count = 0;
    while (true)
    {
        timer_start(frame_timer);

        poll_input(window_manager);
        game_simulate(game_state, frame_dt, window_manager.input, particle_cache, entity_manager);
        game_render(game_state, particle_cache);
        game_end_frame(game_state, entity_manager);

        if (game_state.game_mode == GM_EDITOR) render_debug_ui(window_manager);

        swap_buffers(window_manager);
        timer_stop(frame_timer);
        
        timer_maybe_log_average(frame_timer, frame_count);
        maybe_watch_files(file_watcher, frame_timer.start);

        frame_dt = frame_timer.dt;
        frame_count +=1;
    }

    //@todo: delete glfw?
    logr::report("exiting...");

    return 0;
}

