#ifndef INCLUDE_RENDERER_
#define INCLUDE_RENDERER_
#include <string>
#include <vector>
#include <GL.h>

namespace mdraw
{
    GLenum shader_type_from_extension(const std::string& filename);
}

struct Renderer
{
    // list of shaders?
    // map of shaders?
};

struct Shader
{
    GLuint shader_ID; 
    GLenum shader_type; // resolve shader type from filename
};

class ShaderProgram
{
    std::vector<GLuint> shader_IDs;
    int ID;
}



#endif