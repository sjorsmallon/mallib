/*
    gl_lite.h - Single-header multi-platform OpenGL function loader
    ----------------------------------------------------------------------------
    USAGE
    ----------------------------------------------------------------------------
    1) Add the following lines in exactly one of your cpp files to compile the
       implementation:
           #define GL_LITE_IMPLEMENTATION
           #include "gl_lite.h"
    2) In all other files in which you want to use OpenGL functions, simply 
       include this header file as follows:
           #include "gl_lite.h"
    3) Call gl_lite_init() before using any OpenGL function and after you have a
       valid OpenGL context.
    ----------------------------------------------------------------------------
    LICENSE
    ----------------------------------------------------------------------------
    This software is in the public domain. Where that dedication is not
    recognized, you are granted a perpetual, irrevocable license to copy,
    distribute, and modify this file as you see fit.
*/
#ifndef GL_LITE_H
#define GL_LITE_H

#if defined(__linux__)
#include <dlfcn.h>
#define GLDECL // Empty define
#define MALLIB_GL_LIST_WIN32 // Empty define
#endif // __linux__

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define GLDECL WINAPI

#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
// not sure whether this is needed or not.
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
//
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_VERTEX_SHADER                  0x8B31

#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_UNIFORM                        0x92E1
#define GL_ACTIVE_RESOURCES               0x92F5

#define GL_NAME_LENGTH                    0x92F9
#define GL_TYPE                           0x92FA
#define GL_ARRAY_SIZE                     0x92FB

#define GL_PROGRAM_INPUT                  0x92E3
#define GL_LINK_STATUS                    0x8B82



typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define MALLIB_GL_LIST_WIN32 \
    /* ret, name, params */ \
    GLE(void,      BlendEquation,           GLenum mode) \
    /* GLE(void,      ActiveTexture,           GLenum texture)*/

    /* end */

#endif // _WIN32

#include <GL/gl.h>


#define MALLIB_GL_LIST \
    /* ret, name, params */ \
    GLE(void,      AttachShader,            GLuint program, GLuint shader) \
    GLE(void,      BindBuffer,              GLenum target, GLuint buffer) \
    GLE(void,      BindFramebuffer,         GLenum target, GLuint framebuffer) \
    GLE(void,      BufferData,              GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) \
    GLE(void,      BufferSubData,           GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data) \
    GLE(GLenum,    CheckFramebufferStatus,  GLenum target) \
    GLE(void,      ClearBufferfv,           GLenum buffer, GLint drawbuffer, const GLfloat * value) \
    GLE(void,      CompileShader,           GLuint shader) \
    GLE(GLuint,    CreateProgram,           void) \
    GLE(void,      DeleteProgram,           GLuint program) \
    GLE(GLuint,    CreateShader,            GLenum type) \
    GLE(void,      DeleteBuffers,           GLsizei n, const GLuint *buffers) \
    GLE(void,      DeleteFramebuffers,      GLsizei n, const GLuint *framebuffers) \
    GLE(void,      EnableVertexAttribArray, GLuint index) \
    GLE(void,      DrawBuffers,             GLsizei n, const GLenum *bufs) \
    GLE(void,      FramebufferTexture2D,    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
    GLE(void,      GenBuffers,              GLsizei n, GLuint *buffers) \
    GLE(void,      GenFramebuffers,         GLsizei n, GLuint * framebuffers) \
    GLE(GLint,     GetAttribLocation,       GLuint program, const GLchar *name) \
    GLE(void,      GetShaderInfoLog,        GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    GLE(void,      GetShaderiv,             GLuint shader, GLenum pname, GLint *params) \
    GLE(GLint,     GetUniformLocation,      GLuint program, const GLchar *name) \
    GLE(void,      LinkProgram,             GLuint program) \
    GLE(void,      ShaderSource,            GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) \
    GLE(void,      Uniform1i,               GLint location, GLint v0) \
    GLE(void,      Uniform1f,               GLint location, GLfloat v0) \
    GLE(void,      Uniform2f,               GLint location, GLfloat v0, GLfloat v1) \
    GLE(void,      Uniform4f,               GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
    GLE(void,      UniformMatrix4fv,        GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    GLE(void,      UniformMatrix3fv,        GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    GLE(void,      UseProgram,              GLuint program) \
    GLE(void,      VertexAttribPointer,     GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) \
    GLE(void,      DeleteShader,            GLuint shader) \
    GLE(void,      DetachShader,            GLuint program, GLuint shader) \
    GLE(void,      GenVertexArrays,         GLsizei n, GLuint *arrays) \
    GLE(void,      BindVertexArray,         GLuint array) \
    GLE(void,      Uniform3f,               GLint location, GLfloat v0, GLfloat v1, GLfloat v2) \
    GLE(void,      Uniform3fv,              GLint location, GLsizei count, const GLfloat *value) \
    GLE(void,      Uniform4fv,              GLint location, GLsizei count, const GLfloat *value) \
    GLE(void,      GetProgramInterfaceiv,   GLuint program, GLenum programInterface, GLenum pname, GLint *params)\
    GLE(void,      GetProgramiv,            GLuint program, GLenum pname, GLint *params)\
    GLE(void,      GetProgramResourceiv,    GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params)\
    GLE(void,      GetProgramResourceName,  GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name)\
    GLE(void,      glGetProgramiv,          GLuint program, GLenum pname, GLint *params)\
    GLE(void,      ActiveTexture,           GLenum texture) \
    /* end */

#define GLE(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); extern name##proc * gl##name;
MALLIB_GL_LIST
MALLIB_GL_LIST_WIN32
#undef GLE

bool gl_lite_init();

#endif //GL_LITE_H

// =============================================================================

#ifdef GL_LITE_IMPLEMENTATION

#define GLE(ret, name, ...) name##proc * gl##name;
MALLIB_GL_LIST
MALLIB_GL_LIST_WIN32
#undef GLE

bool gl_lite_init()
{
#if defined(__linux__)

    void* libGL = dlopen("libGL.so", RTLD_LAZY);
    if (!libGL) {
        printf("ERROR: libGL.so couldn't be loaded\n");
        return false;
    }

    #define GLE(ret, name, ...)                                                    \
            gl##name = (name##proc *) dlsym(libGL, "gl" #name);                    \
            if (!gl##name) {                                                       \
                printf("Function gl" #name " couldn't be loaded from libGL.so\n"); \
                return false;                                                      \
            }
        MALLIB_GL_LIST
    #undef GLE

#elif defined(_WIN32)
    OutputDebugStringA("loading opengl functions.\n");     
    
    HINSTANCE dll = LoadLibraryA("opengl32.dll");
    typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);
    if (!dll) {
        OutputDebugStringA("opengl32.dll not found.\n");
        return false;
    }
    wglGetProcAddressproc* wglGetProcAddress =
        (wglGetProcAddressproc*)GetProcAddress(dll, "wglGetProcAddress");

    #define GLE(ret, name, ...)                                                                    \
            gl##name = (name##proc *)wglGetProcAddress("gl" #name);                                \
            if (!gl##name) {                                                                       \
                OutputDebugStringA("Function gl" #name " couldn't be loaded from opengl32.dll\n"); \
                return false;                                                                      \
            }
            else                                                                                   \
            {                                                                                      \
                OutputDebugStringA("Function gl" #name " loaded from opengl32.dll\n");             \
            }                                                                                      \
        MALLIB_GL_LIST
        MALLIB_GL_LIST_WIN32
    #undef GLE

#else
    #error "GL loading for this platform is not implemented yet."
#endif

    return true;
}

#endif //GL_LITE_IMPLEMENTATION