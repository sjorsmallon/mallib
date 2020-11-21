#ifndef INCLUDED_TEXTURE_MANAGER_
#define INCLUDED_TEXTURE_MANAGER_
#include <vector>
#include <map>
#include <string>

//@TODO(Sjors): generalize this to image
struct Texture
{
    uint32_t gl_texture_id; // the actual glgentextures handle returned by openGL.
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
	int next_free_texture_frame = 0; // this is the first time I feel something should be private.
	std::map<std::string, Texture> textures;
	//@Note: eventually, this could be turned into a vector that has gl_texture_id as idx.
};



void load_tga_texture(Texture_Manager& texture_manager, const std::string& texture_name, bool flip_vertically = true);
void load_png_texture(Texture_Manager& texture_manager, const std::string& texture_name, bool flip_vertically = true);

//@FIXME(Sjors): I don't like this (return and do something else.)
// returns the texture ID
uint32_t register_framebuffer_texture(Texture_Manager& texture_manager, const std::string& framebuffer_name);

int32_t get_next_free_texture_frame(Texture_Manager& manager);



#endif