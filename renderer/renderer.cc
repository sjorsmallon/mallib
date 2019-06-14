#include "renderer.ih"

void Renderer::shader_from_file(Shader &shader, const string& filename)
{
    //set Shader
    shader.shader_ID = glCreateShader(shader.shaderType);

    //@cleanup: maybe directly transfer to string?
    File_Handler handler;
    handler.file_to_buffer("shaders/" + filename);
    glShaderSource(shader, 1, &handler.buffer, NULL);
    glCompileShader(shader);
    handler.free_buffer(); // make this RAII?

    GLint vShaderCompiled = GL_FALSE;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &vShaderCompiled);

    if (vShaderCompiled != GL_TRUE)
    {
        SDL_Log("Window::addShaderFromFile: unable to compile vertex shader %d\n", shader);
        printShaderLog(shader);
    }
    else
        SDL_Log("Succesfully added shader to program.");
}