#include "game.h"
#include<fmt/printf.h>
#include <vector>
#include <string>
#include "../sound/sound.h"
#include "../graphics/graphics.h"
#include "../file/file.h"
#include "../font/font.h"
#include "../vec3/vec3.h"
#include "../menu/menu.h"



void game::main_loop()
{
    graphics::clear_buffers();
    
    if (game::global_program_mode() == Program_Mode::GAME)
    {
        // simulate_gameplay();
        // update_game_camera();
    }
    else if (game::global_program_mode() == Program_Mode::MENU)
    {
        menu::draw_menu();
    }
    sound::update_audio();

    graphics::render_frame(); 
    graphics::swap_buffers();

    // graphics::render_frame();
    // if (game::global_program_mode() == Program_Mode::GAME)
    // {
    //     simulate_gameplay();
    //     update_game_camera();
    // }
    // else if(game::global_program_mode() == Program_Mode::EDITOR)
    // {
    //     update_editor();            
    // }
    // update_audio();

}

game::Program_Mode& game::global_program_mode()
{
    static Program_Mode mode;
    return mode;
}

void game::init_font()
{
    font::init_font();
}

void game::init_audio()
{
    sound::init_sound();
}

void game::init_graphics()
{
    graphics::init_graphics();
}

void game::init_everything()
{
    game::init_audio();
    game::init_graphics();
    game::init_font();

    //@TODO: set some modes? program_mode, play the menu music?
    // 
    auto& program_mode = game::global_program_mode();
    program_mode = Program_Mode::MENU;
    sound::play_music("../music/i_feel_so_alive.wav");
    // sound::play_sound("../music/chicken.wav");

    graphics::clear_buffers();
}

void game::shutdown()
{
    sound::perform_shutdown();
}

void game::deinit_everything()
{
    // free all resources.
    //sound::deinit_sound();
    //graphics::deinit_graphics();
    //font::deinit_font();
    
}



// void game::load_models()
// {
//     std::vector<std::string> file_names = file::list_files_in_dir("../data/obj");
//     for (auto& file_name: file_names)
//     {
//         fmt::printf("[game] loading obj file {}", file_name);
//         // renderer::load_obj_file(file_name);
//     }
// }

// void game::load_shaders()
// {
//     std::vector<std::string> file_names = file::list_files_in_dir("../data/shaders");
//     for (auto& file_name: file_names)
//     {
//         fmt::printf("[game] loading shader {}", file_name);
//         // renderer::load_shader(file_name);
//         // graphics::load_and_compile_shader(file_name);
//     }
// }
