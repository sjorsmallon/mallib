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

#include "../input/input.h"
#include <chrono>

void game::init_everything()
{
    game::init_audio();
    game::init_graphics();
    game::init_font();

    //@TODO: set some modes? program_mode, play the menu music?
    auto& program_mode = game::global_program_mode();
    program_mode = Program_Mode::MENU;

    sound::load_music("../music/introduction.mp3");
    sound::play_music("../music/introduction.mp3");

    // uint32_t VBO = 
    // uint32_t VAO = 
    auto& raw_data = graphics::cat_data();
    graphics::load_obj("../object_files/cat.obj", raw_data);
    graphics::generate_vertices_from_raw_data(raw_data);

    graphics::clear_buffers();
}

void game::main_loop()
{
    graphics::clear_buffers();

    // start & end are used for frametime.
    auto start = std::chrono::system_clock::now();
    const auto program_mode = game::global_program_mode();


    if (program_mode == Program_Mode::GAME)
    {
        // handle input -> simulate gameplay
        simulate_gameplay();
    }
    else if (program_mode == Program_Mode::MENU)
    {
        //@Refactor: how do we handle input in the menu?
        game::handle_menu_input();
        // draw 3d stuff for the menu. 
        //
       // menu::draw_menu();
    }

    sound::update_audio();
    graphics::render_frame(); 
    // @FIXME FIXME : drawing menu after render_frame. This is because we want to render font last.
    // eventually, the menu will have its own buffer etcetera.
    menu::draw_menu();
    graphics::swap_buffers();

    // end time calculation.
    auto end = std::chrono::system_clock::now();
    game::previous_frame_time() = end - start;
}

void game::handle_menu_input()
{
    auto &queue = input::input_queue();
    int active_menu_item = static_cast<int>(menu::active_start_menu_item());

    //@Refactor: this is horrible.
    for (auto key : queue)
    {
        if (key == input::Key_Input::KEY_UP)
        {
            if (active_menu_item == menu::Menu_Item::START) {} // do nothing
            else
                active_menu_item = active_menu_item - 1;
            // return;
            sound::play_sound("../music/menu_select.wav");

        }
        if (key == input::Key_Input::KEY_DOWN)
        {
            if(active_menu_item == menu::Menu_Item::EXIT) {} // do nothing
            else
                active_menu_item = active_menu_item + 1;
            sound::play_sound("../music/menu_select.wav");

        }
    }
    menu::active_start_menu_item() = static_cast<menu::Menu_Item>(active_menu_item);
    queue.clear();
}


void game::simulate_gameplay()
{
    //@FIXME: where is the input queue emptied?
    //@FIXME: do we want the keyboard to be bitwise? I think so,
    // Now we are evaluating every value for every key.
    // so we can continue?
    auto &queue = input::input_queue();
    for (auto key : queue)
    {
        if (key == input::Key_Input::KEY_UP)
        {
        }
    }

    queue.clear();
}



std::chrono::duration<float, std::milli>& game::previous_frame_time()
{
    static std::chrono::duration<float, std::milli> previous_frame_time;
    return previous_frame_time;
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

// Jon Blow main loop.
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
