#include "init.h"

#include "camera.h"
#include "entity_system.h"
#include "render_system.h"
#include "shader_manager.h"
#include "texture_manager.h"


void init_all()
{
	// at this point, no user-defined things have been allocated or loaded. 
    Shader_Manager shader_manager{};
    load_shader(shader_manager, "deferred_geometry");
    load_shader(shader_manager, "deferred_lighting");
    load_shader(shader_manager, "lightbox");
    load_shader(shader_manager, "simple_depth");

    Texture_Manager texture_manager{};
    load_png_texture(texture_manager, "metal");


    Render_Manager render_manager{};
    init_renderer(shader_manager, texture_manager, window_width, window_height);


    // Entity_Manager entity_manager{};
    // create_player(entity_manager);
    // create_lights(entity_manager);

}


