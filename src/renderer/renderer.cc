// #include "renderer.ih"



// void mdraw::init_openGL()
// {
//     Window *platform_window = platform_create_window();

// }

// GLenum mdraw::shader_type_from_extension(const string& filename)
// {
//     std::string_view view(filename);
//     view = view.substr(view.find_last_of(".") + 1);

//     if (view == "vertex")
//         return GL_VERTEX_SHADER;
//     else if (view == "fragment")
//         return GL_FRAGMENT_SHADER;
//     else if (view == "geometry")
//         return GL_GEOMETRY_SHADER;
//     else if (view == "tess_control")
//         return GL_TESS_CONTROL_SHADER;
//     else if (view == "tess_evaluation")
//         return GL_TESS_EVALUATION_SHADER;
//     else if (view == "compute")
//         return GL_COMPUTE_SHADER;
//     else
//         return 0;
// }

// bool Renderer::shader_from_file(Shader& shader, const string& filename)
// {
//     bool success = true;
//     // set shader type based on the extension.
//     shader.shaderType = shader_type_from_extension(filename);

//     //set Shader
//     shader.shader_ID = glCreateShader(shader.shaderType);

//     //@cleanup: maybe directly transfer to string?
//     File_Handler handler("shaders/" + filename);

//     glShaderSource(shader, 1, &handler.buffer, NULL);
//     glCompileShader(shader);
//     // handler.free_buffer(); // is now RAII

//     GLint vShaderCompiled = GL_FALSE;

//     glGetShaderiv(shader, GL_COMPILE_STATUS, &vShaderCompiled);

//     if (vShaderCompiled != GL_TRUE)
//     {
//         success = false;
//         // mlog::report_error("shader_from_file: unable to compile vertex shader {}\n", shader);
//         // printShaderLog(shader);
//     }
//     else
//         success = true;
//         // mlog::report_error("Succesfully added shader to program.");
//     return success;
// }