#include "game.h"
#include<fmt/printf.h>
#include <vector>
#include <string>
#include "../sound/sound.h"
#include "../graphics/graphics.h"
#include "../file/file.h"
#include "../font/font.h"
#include "../vec3/vec3.h"


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
    graphics::init_opengl();
}


void game::init_everything()
{
    game::init_audio();
    game::init_graphics();
    game::init_font();
    graphics::clear_buffers();
}

void game::shutdown()
{
    sound::perform_shutdown();
}

void game::main_loop()
{

    graphics::clear_buffers();
    
    
    std::string text = "sample text";
    uint32_t start_x = 500;
    uint32_t start_y = 500;
    Vec3 color;
    color.r = 0.5f;
    color.g = 0.8f;
    color.b = 0.7f;
    float scale = 1.0f;

    font::draw_text(text, start_x, start_y, scale, color);//, Text_Effect effect);
    

    text = "some other text";
    font::draw_text(text, 20, 20, 2.0f, {0.3f, 0.2f, 0.4f});

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

    // graphics::render_frame();

  
}


void game::load_models()
{
    std::vector<std::string> file_names = file::list_files_in_dir("../data/obj");
    for (auto& file_name: file_names)
    {
        fmt::printf("[game] loading obj file {}", file_name);
        // renderer::load_obj_file(file_name);
    }
}

void game::load_shaders()
{
    std::vector<std::string> file_names = file::list_files_in_dir("../data/shaders");
    for (auto& file_name: file_names)
    {
        fmt::printf("[game] loading shader {}", file_name);
        // renderer::load_shader(file_name);
        // graphics::load_and_compile_shader(file_name);
    }
}
