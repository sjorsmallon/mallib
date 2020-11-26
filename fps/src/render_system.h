#ifndef INCLUDED_RENDER_SYSTEM_
#define INCLUDED_RENDER_SYSTEM_
#include "shader_manager.h"
#include "texture_manager.h"
#include "asset_manager.h"
#include "entity_manager.h"

// forward declarations
struct Camera;
struct Particle_Cache;

void init_renderer(Shader_Manager& shader_manager, 
				   Texture_Manager& texture_manager,
				   Asset_Manager& asset_manager,
				   Entity_Manager& entity_manager,

				   const int frame_buffer_width,
				   const int frame_buffer_height);


// per frame
void render(const Camera camera, Particle_Cache& particle_cache);
void render_shadows(const Camera camera, Particle_Cache& particle_cache);





#endif