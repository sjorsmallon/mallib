#ifndef INCLUDED_RENDER_SYSTEM_
#define INCLUDED_RENDER_SYSTEM_
#include "shader_manager.h"
#include "texture_manager.h"
#include "asset_manager.h"
#include "entity_manager.h"

using Render_Primitive = uint32_t; // = GLenum

struct Draw_Request
{   
    Render_Primitive primitive;
    std::vector<glm::vec3> vertices;

    glm::vec4 color;
    glm::vec4 highlight_color;
    bool wireframe;
    float line_width;
};


// forward declarations
struct Camera;
struct Particle_Cache;

void init_render_system(Shader_Manager& shader_manager, 
				   Texture_Manager& texture_manager,
				   Asset_Manager& asset_manager,
				   Entity_Manager& entity_manager,

				   const int frame_buffer_width,
				   const int frame_buffer_height);


// per frame
void render(const Camera camera, Particle_Cache& particle_cache);


void submit_debug_draw_request(Draw_Request& draw_request);





#endif