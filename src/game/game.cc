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


// if it's only functions, how do we deal with state?
// graphics can use a list of shaders
// graphics can take a pointer to a list of shaders?

// void game::graphics_test()
// {

//     // all of this means that graphics is aware of some type of manager.
//     // which I think makes sense.
//     // problem is still the contract that the manager enforces.
//     // How do we make people aware?

//     // sound_manager
//     // auto & camera =  scene_manager->create_camera()
//     // camera.position = {};
//     // camera.orientation = lookat()
//     // canera.near_clip_distance
//     // camera.far_clip_distance 
//     // 
//     Scene_Manager* scene_manager = new Scene_Manager();
//     Camera* camera =  create_camera(scene_manager);
//     camera->position = {};
//     camera->orientation = {};
//     //@note: these should be in Z.
//     camera->near_clip_distance = 1.0; 
//     camera->far_clip_distance = 100.0;
//     camera->aspect_ratio  = globals->window_width / globals->window_height;
//     camera->field_of_view = 90.0f;
//     // communicate the use of more than one camera?

//     // new idea: the construction of the graphics manager preps the graphics environment.
//     // (i.e. what is not called init_opengl.)
//     // but what does graphics offer?
//     // we can register shaders via the graphics manager
//     // and .....? request VBOs and VAOs?
//     // manages the texture state?
//     // keep the manager a compositional type?
//     Graphics_Manager* graphics_manager = new Graphics_Manager();
//     create_shader();
//     load_compile_attach_shader();
//     create_shader();
//     load_compile_attach_shader();
//     create_shader();
//     load_compile_attach_shader();
//     load_compile_attach_shader();
//     load_compile_attach_shader();
//     load_compile_attach_shader();
//     create_shader();


//     // scene_manager->add_light()
//     // 


//     Shader_Program;
//     load_compile_attach_shader
//     load_compile_attach_shader
//     bind
//     detach_shader
//     detach_shader
//     graphics::shaders.push_back()
//     // implicit connection between font and graphics.
//     // can we use font without graphics?
//     font VAO
//     font VBO
//     graphics::left_handed_3d();

// }



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


   // generate the VAO/VBO map.
    auto& buffers =  graphics::buffers();

    for (auto &[key, raw_object_data]: asset::obj_data())
    {

        graphics::Buffers new_buffer= {};
        glGenVertexArrays(1, &new_buffer.VAO);
        glBindVertexArray(new_buffer.VAO);
        glGenBuffers(1, &new_buffer.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, new_buffer.VBO);
        const uint32_t pos_array = 0;
        const uint32_t uv_array = 1;
        const uint32_t normals_array = 2;
        glEnableVertexAttribArray(pos_array);
        glEnableVertexAttribArray(uv_array);
        glEnableVertexAttribArray(normals_array);
        glVertexAttribPointer(pos_array,     3, GL_FLOAT, GL_FALSE, sizeof(asset::Vertex), 0); // x, y, z
        glVertexAttribPointer(uv_array,      2, GL_FLOAT, GL_FALSE, sizeof(asset::Vertex), ((void*)(3 * sizeof(float)))  ); // skip  3: u, v,
        glVertexAttribPointer(normals_array, 3, GL_FLOAT, GL_FALSE, sizeof(asset::Vertex), ((void*)(5 * sizeof(float)))  ) ; // skip 5: nx, ny, nz.
        glBufferData(GL_ARRAY_BUFFER,
                    static_cast<int>(raw_object_data.vertices.size() * sizeof(asset::Vertex)),
                    raw_object_data.vertices.data(),
                    GL_STATIC_DRAW);
        glBindVertexArray(0);
        buffers[key] = new_buffer;
    }

    for (auto &[key,value]: buffers)
    {
        fmt::print("{}: buffer.", key);
    }


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

    // rudimentary frame time calculation. Eventually, we want this to be in double form.
    auto end = std::chrono::system_clock::now();
    game::previous_frame_time() = end - start;
}

void game::handle_menu_input()
{
    auto &queue = input::input_queue();
    int active_menu_item = static_cast<int>(menu::active_start_menu_item());

    //@Refactor: look up how to properly deal with key events.
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

