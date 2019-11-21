#include "game.h"
#include<fmt/printf.h>
#include <vector>
#include <string>

#include "../sound/sound.h"
#include "../graphics/graphics.h"
#include "../asset/asset.h"
#include "../file/file.h"
#include "../font/font.h"
#include "../vec3/vec3.h"
#include "../menu/menu.h"
#include "../input/input.h"

#include <chrono>

// Init -> Load -> main_loop.
void game::init_everything()
{
    // inits
    sound::init_sound();
    graphics::init_graphics();
    font::init_font();
    menu::init_menu();



    //@TODO: set some modes? program_mode, play the menu music?
    auto& program_mode = game::global_program_mode();
    program_mode = Program_Mode::MENU;

    game::load_everything(); 
    graphics::clear_buffers();
}

void game::load_everything()
{
    // load sound
    // sound::load_music("assets/music/introduction.mp3");
    // sound::play_music("assets/music/introduction.mp3");


    // load assets
    asset::Asset_Folders asset_folders = {};
    asset_folders.obj_folder = "assets/object_files/";
    asset_folders.mtl_folder = "assets/mtl_files/";
    asset_folders.texture_folder = "assets/texture_files/";
    asset_folders.scene_folder = "assets/scene_files/";

    asset::load_assets_from_file(asset_folders);

    // let's try to create a scene.
    graphics::active_scene() = asset::scenes()["test.scene"];
    
    for (auto& set_piece: asset::scenes()["test.scene"].set_pieces)
    {
        fmt::print("set_piece name: {}\n", set_piece.name);
        fmt::print("model name name: {}\n", set_piece.model_name);
    }
    graphics::init_texture_settings(asset::texture_data());
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
            sound::play_sound("assets/music/menu_select.wav");

        }
        if (key == input::Key_Input::KEY_DOWN)
        {
            if(active_menu_item == menu::Menu_Item::EXIT) {} // do nothing
            else
                active_menu_item = active_menu_item + 1;
            sound::play_sound("assets/music/menu_select.wav");

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

