#include "texture_manager.h"
#include "logr.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

//@Temporary:
#include "timed_function.h"

#include "logr.h"


namespace
{
	std::string g_texture_folder_prefix{"../assets/textures/"};
	const std::string g_texture_png_extension_suffix{".png"};
    const std::string g_texture_tga_extension_suffix{".tga"};


}

void set_texture_path(Texture_Manager& texture_manager, const char* texture_folder_prefix)
{
    g_texture_folder_prefix = texture_folder_prefix;
}

// returns the texture id
uint32_t register_framebuffer_texture(Texture_Manager& texture_manager, const std::string& framebuffer_name)
{
    logr::report("register_framebuffer_texture: {}\n", framebuffer_name);
    //@FIXME(Sjors): implicit creation
    auto& texture = texture_manager.textures[framebuffer_name];
    glGenTextures(1, &texture.gl_texture_id);
    texture.gl_texture_frame = get_next_free_texture_frame(texture_manager);
    
    return texture.gl_texture_id;
}

void load_tga_texture(Texture_Manager& texture_manager, const std::string& texture_name)
{
    //@FIXME(Sjors): implicit creation
    auto& texture = texture_manager.textures[texture_name];

    std::string file_path = g_texture_folder_prefix + texture_name + g_texture_tga_extension_suffix;
    
    stbi_set_flip_vertically_on_load(true);

    texture.data = stbi_load(file_path.c_str(),
        &texture.dimensions[0], // x_dim
        &texture.dimensions[1], // y_dim
        &texture.channels,
        STBI_rgb);

    if (texture.data == nullptr)
    {
        logr::report_error("[Texture_Manager] failed loading texture \"{}\". stbi_error: {}\n", texture_name, stbi_failure_reason());
        return;
    }
    else 
    {
        texture.data_size = texture.dimensions[0] * texture.dimensions[1] * texture.channels;
        logr::report("[Texture_Manager] loaded texture {}.\n", file_path);
    }

    texture.gl_texture_frame = get_next_free_texture_frame(texture_manager);
    glActiveTexture(GL_TEXTURE0 + texture.gl_texture_frame);
    glGenTextures(1, &texture.gl_texture_id);

    glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        texture.dimensions[0], //x
        texture.dimensions[1], //y
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        texture.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
}


void load_alpha_png_texture(Texture_Manager& texture_manager, const std::string& texture_name)
{
     //@FIXME(Sjors): implicit creation
    auto& texture = texture_manager.textures[texture_name];
    std::string file_path = g_texture_folder_prefix + texture_name + g_texture_png_extension_suffix;
    
    stbi_set_flip_vertically_on_load(true);

    texture.data = stbi_load(file_path.c_str(),
        &texture.dimensions[0], // x_dim
        &texture.dimensions[1], // y_dim
        &texture.channels,
        STBI_rgb_alpha);

    if (texture.data == nullptr)
    {
        logr::report_error("[Texture_Manager] failed loading texture \"{}\". stbi_error: {}\n", texture_name, stbi_failure_reason());
        exit(1);
        return;
    }
    else 
    {
        texture.data_size = texture.dimensions[0] * texture.dimensions[1] * texture.channels;
        logr::report("[Texture_Manager] loaded texture {}.\n", file_path);
    }

    // bind texture.
    {
        glGenTextures(1, &texture.gl_texture_id);
        texture.gl_texture_frame = get_next_free_texture_frame(texture_manager);

        glActiveTexture(GL_TEXTURE0 + texture.gl_texture_frame);
        glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            texture.dimensions[0], //x
            texture.dimensions[1], //y
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            texture.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}


void load_png_texture(Texture_Manager& texture_manager, const std::string& texture_name)
{
    //@FIXME(Sjors): implicit creation
    auto& texture = texture_manager.textures[texture_name];
    std::string file_path = g_texture_folder_prefix + texture_name + g_texture_png_extension_suffix;
    
    stbi_set_flip_vertically_on_load(true);

    texture.data = stbi_load(file_path.c_str(),
        &texture.dimensions[0], // x_dim
        &texture.dimensions[1], // y_dim
        &texture.channels,
        STBI_rgb);

    if (texture.data == nullptr)
    {
        logr::report_error("[Texture_Manager] failed loading texture \"{}\". stbi_error: {}\n", texture_name, stbi_failure_reason());
        return;
    }
    else 
    {
        texture.data_size = texture.dimensions[0] * texture.dimensions[1] * texture.channels;
        logr::report("[Texture_Manager] loaded texture {}.\n", file_path);
    }

    // bind texture.
    {
        glGenTextures(1, &texture.gl_texture_id);
        texture.gl_texture_frame = get_next_free_texture_frame(texture_manager);

        glActiveTexture(GL_TEXTURE0 + texture.gl_texture_frame);
        glBindTexture(GL_TEXTURE_2D, texture.gl_texture_id);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB8,
            texture.dimensions[0], //x
            texture.dimensions[1], //y
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            texture.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

}

int32_t get_next_free_texture_frame(Texture_Manager& manager)
{
    assert(manager.next_free_texture_frame < 32);
	int32_t to_return = manager.next_free_texture_frame;
	manager.next_free_texture_frame += 1;
	return to_return;
}
