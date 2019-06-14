#ifndef INCLUDE_RENDERER_
#define INCLUDE_RENDERER_
#include <string>
#include <vector>

struct Renderer
{
    // list of shaders?
    // map of shaders?
};

struct Shader
{
    GLuint shader_ID; 
    GLenum shader_type; // resolve shader type from filename? vertex / pixel?
    // std::string data; is this necessary?
};

class ShaderProgram
{
    std::vector<GLuint> shader_IDs;
    int ID;
}



#endif