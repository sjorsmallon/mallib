#include <graphics.h>

#undef max
#undef min
#include <fmt/core.h>
#include <filesystem> // for directory iteration for the shader.
#include <stdint.h>
#include <string>
#include <sstream>
#include <Wingdi.h>
#include <stdlib.h>

//--- OpenGL
#include <gl_lite/gl_lite.h>

//--- Imgui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_opengl3.h>

// common / shared / global
#include <globals.h>
#include <cvar_system.h>


//--- file includes to sort out.
#include <on_leaving_scope.h>
#include <file.h>
#include <scene.h>
#include <game.h> // game time?
#include <camera.h>

#include <mgl/mat.h>
#include <mgl/mat4.h>
#include <mgl/mat3.h>

#include <glm/gtx/string_cast.hpp> // to_string
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>   // rotation_matrix 

#define BUFFER_OFFSET(i) ((void*)(i)) //hacky macro for offset.

namespace fs = std::filesystem;

cam::Camera& graphics::active_camera()
{
    static cam::Camera active_camera;
    return active_camera;
}

std::map<std::string, graphics::Buffers>& graphics::buffers()
{
    static std::map<std::string, graphics::Buffers> buffers;
    return buffers;
}

void graphics::init_graphics()
{ 
    graphics::global_Win32_context().device_context = globals.device_context;
    graphics::global_Win32_context().window_handle = globals.window_handle;

    graphics::active_camera() = cam::default_camera();
    graphics::init_opengl();
    graphics::init_imgui();
    graphics::clear_buffer_bits();

    graphics::init_cvars();
}

// static
void graphics::init_opengl()
{
    //@NOte::init gl_lite only after the gl_context has been created
    // (which is done in the win32 section of the program, since that is OS related.)
    gl_lite_init();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(40.0f/255.0f, 45.0f/255.0f, 54.0f/ 255.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//static
void graphics::init_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(graphics::global_Win32_context().window_handle);
    std::string glsl_version = "#version 430";
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());

}




void graphics::init_texture_settings(std::map<std::string, asset::Texture>& textures)
{
    for (auto& [texture_name, texture]: textures)
    {
        glGenTextures(1, &texture.gl_texture_id);

        uint32_t free_texture_frame = graphics::next_free_active_texture_ID();
        glActiveTexture(GL_TEXTURE0 + free_texture_frame);
        glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
       
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB8,
            texture.dimensions.x,
            texture.dimensions.y,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            texture.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    //@TODO: generate mipmaps.
}


// static
void graphics::clear_buffer_bits()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void graphics::render_frame()
{
    graphics::render_game_3d();
}


scene::Scene& graphics::active_scene()
{
    static scene::Scene active_scene;
    return active_scene;
}




// Jon Blow's render_frame call.
// void graphics::render_frame()
// {
    
//     graphics::set_render_target(0, hdr_buffer);
//     graphics::set_depth_target(0, hdr_depth);

//     glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     graphics::draw_game_3d();

//     graphics::set_render_target(0, back_buffer);
//     graphics::set_depth_target(0, nullptr); // clear_depth_target;

//     graphics::resolve_to_ldr();

//     glEnable(GL_BLEND);
//     Hud_Properties hud = {};
//     hud.start_x = 100; 
//     hud.start_y = 100;
//     hud.width   = 500;
//     hud.height  = 500; 

//     graphics::draw_hud(hud);

//     graphics::swap_buffers();

// }