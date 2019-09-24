#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H 
#include <fmt/core.h>

std::map<GLchar, Character>& font::characters()
{
	static std::map<GLchar, Character> characters;
	return characters;
}

// void font::init_freeType()
// {
// 	FT_library ft;
// 	if (FT_Init_FreeType(&ft))
// 	{
// 		fmt::print("init_freetype: failed to init freetype.");
// 	}
// 	FT_Face face;
// 	if (FT_New_Face(ft, "..fonts/karminabold.otf", 0, &face))
// 		fmt::print("new_face: failed to create new font face.");

// 	FT_Set_Pixel_Sizes(face, 0, 48);
// }


void font::init_font()
{
	FT_library ft;
	if (FT_Init_FreeType(&ft))
	{
		fmt::print("init_freetype: failed to init freetype.");
	}
	FT_Face face;
	if (FT_New_Face(ft, "..fonts/karminabold.otf", 0, &face))
		fmt::print("new_face: failed to create new font face.");

	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction?

	for (glubyte char_index = 0; char_index != 128;  ++char_index)
	{
		if (FT_Load_Char(face, char_index, FT_LOAD_RENDER))
		{
			fmt::print("error: freetype: failed to load glyph %", char_index);
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
			);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character_glyph = {
								texture,
				 				{face->glyph->bitmap.width, face->glyph->bitmap.rows},
				 				{face->glyph->bitmap_left, face->glyph->bitmap_top},
				 				face->glyph->advance.x
				 			  };

		font::characters().insert(std::pair<GLchar, Character>(char_index, character_glyph));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}


