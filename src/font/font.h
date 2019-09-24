#ifndef INCLUDED_FONT_
#define INCLUDED_FONT_

#include <ft2build.h>
#include FT_FREETYPE_H 
#include <map>

struct Character
{
	GLuint textureID;
	Vec2 Size;
	Vec2 Bearing;
	GLuint advance;
};

namespace font
{

	std::map<GLchar, Character>& characters();

	void init_freetype();
	void init_font();
	
	
};

#endif
