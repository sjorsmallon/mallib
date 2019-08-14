#ifndef INCLUDE_RENDERER_
#define INCLUDE_RENDERER_
#include <string>
#include <vector>



struct Platform_Renderer_Limits
{
	uint32_t max_quads_per_frame;
}



struct Platform_Renderer
{
	Renderer_Texture_Queue texture_queue;
}


// namespace window; // i think

// namespace renderer
// {
//     GLenum shader_type_from_extension(const std::string& filename);
    
//     void init_opengl();
// }


// struct Renderer
// {
//     // list of shaders?
//     // map of shaders?
// };

// struct Shader
// {
//     GLuint shader_ID; 
//     GLenum shader_type; // resolve shader type from filename
// };

// class ShaderProgram
// {
//     std::vector<GLuint> shader_IDs;
//     int ID;
// }



#endif