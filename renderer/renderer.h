#ifndef INCLUDE_RENDERER_
#define INCLUDE_RENDERER_
#include <string>

struct Renderer
{
    // list of shaders?
    // map of shaders
};

struct Shader
{
    GLuint shader_ID;
    GLenum shader_type;
    std::string data;
};


#endif