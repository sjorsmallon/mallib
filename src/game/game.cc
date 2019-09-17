#include "game.h"
#include<fmt/printf.h>
#include <vector>
#include <string>
#include "../sound/sound.h"
#include "../graphics/graphics.h"
#include "../file/file.h"

// @Think about this pattern of loading. We hide where it loads to.
// That does not help in this case. All submodules now  need to be aware 
// about where this data stays.
// using std::vector<std::string>> File_List;


void game::audio_setup()
{
    sound::perform_setup();
}

void game::shutdown()
{
    sound::perform_shutdown();
}


void game::main_loop()
{
    graphics::draw_frame();
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
    }
}
