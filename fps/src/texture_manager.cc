#include "texture_manager.h"
#include <log/log.h>
#include <glad/glad.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace
{
	std::string g_texture_folder_prefix = "assets/textures/";
	std::string g_texture_extension_suffix = ".png";
}

// returns the texture id
uint32_t register_framebuffer_texture(Texture_Manager& texture_manager, const std::string& framebuffer_name)
{
    //@FIXME(Sjors): implicit creation
    auto& texture = texture_manager.textures[framebuffer_name];
    glGenTextures(1, &texture.gl_texture_id);
    texture.gl_texture_frame = get_next_free_texture_frame(texture_manager);

    return texture.gl_texture_id;
}

void load_png_texture(Texture_Manager& texture_manager, const std::string& texture_name, bool flip_vertically)
{
    //@FIXME(Sjors): implicit creation
    auto& texture = texture_manager.textures[texture_name];

    std::string file_path = g_texture_folder_prefix + texture_name + g_texture_extension_suffix;
    logr::report("[Texture_Manager] file path: {}\n", file_path);
    // stbi_set_flip_vertically_on_load(true);

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

int32_t get_next_free_texture_frame(Texture_Manager& manager)
{
	int32_t to_return = manager.next_free_texture_frame;
	manager.next_free_texture_frame += 1;
	return to_return;
}
