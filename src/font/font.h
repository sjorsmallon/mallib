#ifndef INCLUDED_FONT_
#define INCLUDED_FONT_
#include <ft2build.h>
#include FT_FREETYPE_H 

#include <map>

#include "../graphics/gl_lite.h"
#include "../vec2/vec2.h"
#include "../vec3/vec3.h"

//@Todo: implement a font selector for draw_text?
// Character is now a set of glyphs for one particular font.




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

	gl_Objects& gl_objects();

	std::map<char, Character>& characters();

	void init_font();
	void init_font_gl_objects();
	void gl_text_mode();
	void draw_text(std::string& text,
				   // Font font,
				   uint32_t start_x,
				   uint32_t start_y,
				   float scale,
				   Vec3 color);
					   // Text_Effect effect);


	
	
};

#endif
