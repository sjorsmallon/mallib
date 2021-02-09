#ifndef INCLUDED_TEXTURE_MANAGER_
#define INCLUDED_TEXTURE_MANAGER_
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <glm/vec2.hpp>
#include "logr.h"

struct Texture_Atlas
{
    int32_t gl_texture_frame;
    uint32_t gl_texture_id;

    std::map<std::string, glm::vec2> texture_offset;

};

struct Texture
{
    uint32_t gl_texture_id; // the actual glgentextures handle returned by openGL that this texture is bound to.
    int32_t gl_texture_frame; // the GL_TEXTUREN value.

    unsigned char* data = nullptr; // hmm. this is malloc'd by stb_img. we call free in the destructor.
    size_t data_size;
    int  dimensions[2]; // x, y
    int       channels; // number of color channels (3 for RGB, 4 for RGBA)
    
    ~Texture()
    {
    	if (data != nullptr)
    	{
            free(data);
    	}
    }
};


struct Texture_Manager
{
	int next_free_texture_frame = 0;
	std::map<std::string, Texture> textures;
};

void set_texture_path(Texture_Manager& texture_manager, const char* texture_folder_prefix);


void load_tga_texture(Texture_Manager& texture_manager, const std::string& texture_name);
void load_png_texture(Texture_Manager& texture_manager, const std::string& texture_name);
void load_alpha_png_texture(Texture_Manager& texture_manager, const std::string& texture_name);



//@FIXME(Sjors): I don't like this (return and do something else.)
// returns the texture ID
uint32_t register_framebuffer_texture(Texture_Manager& texture_manager, const std::string& framebuffer_name);

int32_t get_next_free_texture_frame(Texture_Manager& manager);

inline const Texture& get_texture(Texture_Manager& texture_manager, const std::string& texture_name)
{
    if (texture_manager.textures.find(texture_name) == texture_manager.textures.end())
    {
        logr::report_error("[texture_manager] could not find {}\n",  texture_name);
        for (auto& [key, value] : texture_manager.textures)
        {
            logr::report("key, [{}]\n", key);
        }
        assert(texture_manager.textures.find(texture_name) == texture_manager.textures.end());
    }    


    

    return texture_manager.textures[texture_name];
}



#endif