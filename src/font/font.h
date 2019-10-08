#ifndef INCLUDED_FONT_
#define INCLUDED_FONT_

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <array> 
#include <string> // for testing.
#include <vector>

#include "../graphics/gl_lite.h"
#include "../vec2/vec2.h"
#include "../vec3/vec3.h"

//@Todo: for now, only one font lives in here.
// provide the user a pointer that they can set 
// to some particular font for all these functions?

namespace font
{
	struct Character
	{
		GLuint textureID;
		Vec2i size;
		Vec2i bearing;
		GLuint advance;
	};
	
	struct gl_Objects
	{
		GLuint VAO;
		GLuint VBO;
	};

	struct Font
	{
		std::vector<Character> characters;
	};

	gl_Objects& gl_objects();

	// std::map<char, Character>& characters();
	// std::vector<Character>& characters();
	std::array<Character, 128>& characters();

	void init_font();
	void init_font_gl_objects();
	void gl_text_mode();
	void draw_text(std::string text,
				   // Font font,
				   uint32_t start_x,
				   uint32_t start_y,
				   float scale,
				   Vec3 color);
					   // Text_Effect effect);

	uint32_t get_string_width_in_pixels(Font& font,std::string text); // font effect?

	void generate_font_at_size(std::vector<Character>& target, std::string& filename, uint32_t pixel_size);

};

#endif
