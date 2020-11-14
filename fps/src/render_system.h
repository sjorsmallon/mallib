#ifndef INCLUDED_RENDER_SYSTEM_
#define INCLUDED_RENDER_SYSTEM_
#include "shader_manager.h"
#include "texture_manager.h"

// forward declarations
struct Camera;
struct Particle_Cache;

// struct Draw_Data
// {
// 	gluint VAO;
// 	gluint VBO;
// 	gluint texture_id;
// };

enum Key
{
	KEY_W = 0,
	KEY_A = 1,
	KEY_S = 2,
	KEY_D = 3
};



void init_renderer(Shader_Manager& shader_manager, 
				   Texture_Manager& texture_manager,
				   const int frame_buffer_width,
				   const int frame_buffer_height);


// per frame
void render(const Camera camera, Particle_Cache& particle_cache);



void update_player_camera_with_mouse_input(const float x_offset, const float y_offset, const bool should_constrain_pitch = true);
void update_player_position_with_key_input(const Key pressed_key);



#endif