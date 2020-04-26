#include <game.h>

#include<fmt/printf.h>
#include <vector>
#include <string>

#include <sound.h>
#include <graphics.h>
#include <asset.h>
#include <file.h>
#include <font.h>
#include <menu.h>
#include <io.h>
#include <chrono> // for frame_time.

// these functions are expected to be called in the entrypoint.
// Init -> Load -> main_loop.

void game::init()
{
    // init
    graphics::init_graphics();
    sound::init_sound();
    font::init_font();
    menu::init_menu();
    // load
    game::load_assets();
    game::load_shaders();
    // main_loop
}

void game::load_shaders()
{ 
    uint32_t text_shader                = graphics::load_shader("text");
    uint32_t cel_shader                 = graphics::load_shader("cel");
    uint32_t isophotes_shader           = graphics::load_shader("isophotes");
    uint32_t normals_shader             = graphics::load_shader("normals");  
    uint32_t line_position_color_shader = graphics::load_shader("line_position_color");
    
    graphics::shaders()["text"]      = text_shader;
    graphics::shaders()["cel"]       = cel_shader;
    graphics::shaders()["isophotes"] = isophotes_shader;
    graphics::shaders()["normals"]   = normals_shader;
    graphics::shaders()["line_position_color"] = line_position_color_shader;

    graphics::set_shader("cel");

}

void game::load_assets()
{
    // load sound
    // sound::load_music("assets/music/introduction.mp3");
    // sound::play_music("assets/music/introduction.mp3");
    // load assets
    asset::Asset_Folders asset_folders = {};
    asset_folders.obj_folder     = "assets/object_files/";
    asset_folders.mtl_folder     = "assets/mtl_files/";
    asset_folders.texture_folder = "assets/texture_files/";
    asset_folders.scene_folder   = "assets/scene_files/";

    asset::load_assets_from_file(asset_folders);

    graphics::active_scene() = asset::scenes()["test.scene"];  // framegraph?
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

    //@FIXME: this is a 
    graphics::Buffers new_buffer{};
    glGenVertexArrays(1, &new_buffer.VAO);
    glBindVertexArray(new_buffer.VAO);
    glGenBuffers(1, &new_buffer.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, new_buffer.VBO);
    const uint32_t position_idx = 0;
    const uint32_t color_idx = 1;
    glEnableVertexAttribArray(position_idx);
    glEnableVertexAttribArray(color_idx);

    const uint32_t position_element_count = 4;
    const uint32_t color_element_count = 4;

    glVertexAttribPointer(position_idx, position_element_count, GL_FLOAT, GL_FALSE, 2 * sizeof(mgl::vec4), 0);
    glVertexAttribPointer(color_idx, color_element_count, GL_FLOAT, GL_FALSE, 2 * sizeof(mgl::vec4), ((void*)(sizeof(mgl::vec4))) ); // skip p.x, p.y,p.z, p.w;

    glBindVertexArray(0);
    buffers["line_position_color"] = new_buffer;


}

void game::main_loop()
{
    auto start = std::chrono::system_clock::now();

    graphics::clear_buffer_bits();
    const auto program_mode = Program_Mode::DEBUG;

    if (program_mode == Program_Mode::GAME)
    {
        simulate_gameplay();
    }
    else if (program_mode == Program_Mode::MENU)
    {
        game::handle_menu_input();
    }
    else if (program_mode == Program_Mode::DEBUG)
    {
        game::handle_debug_input();
    }

     sound::update_audio();

    // @FIXME FIXME : drawing menu after render_frame. This is because we want to render font last.
    graphics::render_frame(); 
    graphics::render_ui();
    graphics::swap_buffers();
   
    // rudimentary frame time calculation. This can be replaced by an OpenGL query.
    auto end = std::chrono::system_clock::now();
    // global::globals().previous_frame_time = end - start;
}


void start_frame()
{
    // wipe memory?
    // 
}

void finish_frame()
{
    // clear input queue & mouse.
}

void game::handle_debug_input()
{
    auto& queue = io::input_queue();
    auto& mouse_state = io::mouse_state();

    graphics::update_active_camera(mouse_state);

    for (auto key : queue)
    {
       
    }
    // menu::active_start_menu_item() = static_cast<menu::Menu_Item>(active_menu_item);
    queue.clear();

    //@FIXME: reset mouse scroll delta.
    mouse_state.scroll_delta_y = double{0.0};
}


void game::handle_menu_input()
{
    auto &queue = io::input_queue();
    int active_menu_item = static_cast<int>(menu::active_start_menu_item());


    for (auto key : queue)
    {
        if (key == io::Button::KEY_UP)
        {
            if (active_menu_item == menu::Menu_Item::START) {} // do nothing
            else
                active_menu_item = active_menu_item - 1;
            // return;
            sound::play_sound("assets/music/menu_select.wav");

        }
        if (key == io::Button::KEY_DOWN)
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
    auto &input_queue = io::input_queue();
    for (auto key : input_queue)
    {
        if (key == io::Button::KEY_UP)
        {
        }
    }

    // clear input queue.
    input_queue.clear();
}


std::chrono::duration<double, std::milli>& game::previous_frame_time()
{
    static std::chrono::duration<double, std::milli> previous_frame_time;
    return previous_frame_time;
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

// game::Program_Mode& game::global_program_mode()
// {
//     static Program_Mode mode;
//     return mode;
// }


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

std::map<std::string, uint32_t>& game::shaders()
{ 
    static std::map<std::string, uint32_t> shaders;
    return shaders;
}