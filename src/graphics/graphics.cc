#include "graphics.h"
#undef max
#undef min
#include <fmt/core.h>
#include <fmt/printf.h>
#include <GL/GL.h>
#include <Wingdi.h>
#include <stdlib.h>

//win32




graphics::Win32_Context& graphics::global_Win32_context()
{
	static Win32_Context context;
	return context;
}

void graphics::render_frame()
{
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	graphics::swap_buffers();
	// invoke swap_buffers() on your own platform?
}

void graphics::swap_buffers()
{
	HDC device_context = graphics::global_Win32_context().device_context;
	SwapBuffers(device_context);
}


GLenum graphics::shader_type_from_extension(const string& filename)
{
    std::string_view view(filename);
    view = view.substr(view.find_last_of(".") + 1);

    if (view == "vertex")
        return GL_VERTEX_SHADER;
    else if (view == "fragment")
        return GL_FRAGMENT_SHADER;
    else if (view == "geometry")
        return GL_GEOMETRY_SHADER;
    else if (view == "tess_control")
        return GL_TESS_CONTROL_SHADER;
    else if (view == "tess_evaluation")
        return GL_TESS_EVALUATION_SHADER;
    else if (view == "compute")
        return GL_COMPUTE_SHADER;
    else
        return 0;
}

bool graphics::load_and_compile_shader(const string& filename)
{
    bool success = true;
    // set shader type based on the extension.
    GLenum shaderType = shader_type_from_extension(filename);

    //set Shader
    GLuint shader_ID = glCreateShader(shader.shaderType);

    //@cleanup: maybe directly transfer to string?
    std::string target = {};
    file::file_to_string(filename, target);

    glShaderSource(shader, 1, &target.c_str(), NULL);
    glCompileShader(shader);
    // handler.free_buffer(); // is now RAII

    GLint vShaderCompiled = GL_FALSE;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &vShaderCompiled);

    if (vShaderCompiled != GL_TRUE)
    {
        success = false;
        fmt::print("shader_from_file: unable to compile vertex shader {}\n", shader);
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.

        // Exit with failure.
        glDeleteShader(shader); // Don't leak the shader.
        return;
        // printShaderLog(shader);
    }
    else
    {
        success = true;
    }

    return success;
}