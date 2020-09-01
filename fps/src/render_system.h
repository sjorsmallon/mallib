#ifndef INCLUDED_RENDER_SYSTEM_
#define INCLUDED_RENDER_SYSTEM_
#include <entt/entt.h>
#include "shader_manager.h"
#include "texture_manager.h"

// struct Draw_Data
// {
// 	gluint VAO;
// 	gluint VBO;
// 	gluint texture_id;
// };

// once 
void init_renderer(Shader_Manager& shader_manager, 
				   Texture_Manager& texture_manager,
				   const int frame_buffer_width,
				   const int frame_buffer_height);


// per frame
void render();



#endif