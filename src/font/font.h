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


//@Todo: for now, a default font lives in the namespace.

namespace font
{
	//@Todo: for now, colour_shift lerps the colour between white and the chosen colour.

	enum class Text_Effect
	{
		NONE, 
		COLOUR_SHIFT, // shifts color between white & chosen color. what is the time duration?
		HORIZONTAL_SHIFT, // shift color horizontally
		VERTICAL_SHIFT, // shift color vertically,
		SHADOWED // draw "shadows (double layered"). @Todo: offset direction?
	};

	struct Character
	{
		GLuint textureID;
		Vec2i size;
		Vec2u bearing;
		int32_t advance;
	};

	struct Font
	{
		float scale;
		float pixel_count; // both x,y.		
		std::vector<Character> characters;
		Font() = default;
		~Font()
		{
			for(const auto& character: characters)
				glDeleteTextures(1, &character.textureID);
		}

		Font(const Font&) = delete; // no copy constructing.
		Font& operator=(const Font&) = delete; // no assignment.
		Font& operator=(Font&&) = delete; // no moving / stealing!

	};
	
	struct gl_Objects
	{
		GLuint VAO;
		GLuint VBO;
	};


	gl_Objects& gl_objects();

	Font& default_font();
	// std::array<Character, 255>& default_font();

	void init_font();
	void init_font_gl_objects();
	void gl_text_mode();
	void draw_text(std::string text,
				   Font& font,
				   uint32_t start_x,
				   uint32_t start_y,
				   float scale,
				   Vec3 color,
					Text_Effect effect = Text_Effect::NONE
				   );


	void generate_font_at_size(font::Font& target, std::string filename, const uint32_t pixel_count);
	uint32_t get_string_width_in_pixels(const std::string text,font::Font& font, const uint32_t scale = 1.0f);


};

#endif
