#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <fmt/core.h>
#include "../graphics/gl_lite.h"
#include "../graphics/graphics.h"

#include "../vec2/vec2.h"
#include "../mat4/mat4.h"
#include "../mmat/mmat.h"


// std::map<char, font::Character>& font::characters() // does this constitute a font then?
// {
// 	static std::map<char, font::Character> characters;
// 	return characters;
// }
// std::vector<font::Character>& font::characters()
// {
// 	static std::vector<font::Character> characters(128);
// 	return characters;
// }


std::array<font::Character, 128>& font::characters()
{
	static std::array<font::Character,128> characters;
	return characters;
}

font::gl_Objects& font::gl_objects() //@Note:VAO & VBO
{
	static gl_Objects objects;
	return objects;
}



void font::init_font()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		fmt::print("init_freetype: failed to init freetype.");

	FT_Face face;

	// if (FT_New_Face(ft, "../fonts/karminabold.otf", 0, &face))
	// 	fmt::print("new_face: failed to create new font face.\n");

	if (FT_New_Face(ft, "../fonts/arial.ttf", 0, &face))
		fmt::print("new_face: failed to create new font face.\n");

	// if (FT_New_Face(ft, "../fonts/opensans.ttf",0, &face))
		// fmt::print("new_face: failed to create new font face.\n");

	// size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);
	// disable byte-alignment restriction?
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	// create character glyphs and store them in the array.
	for (uint8_t char_index = 0; char_index != 128;  ++char_index)
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

		fmt::print("created Gluint texture: {}", texture);
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

		// font::characters().insert(std::pair<GLchar, Character>(char_index, character_glyph));
		font::characters()[char_index] = character_glyph;
	}
	glBindTexture(GL_TEXTURE_2D, 0); // this is the texture ID. we should store this as well.

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	font::init_font_gl_objects();
}

void font::init_font_gl_objects()
{
	font::gl_Objects& objects = font::gl_objects();

	glGenVertexArrays(1, &objects.VAO);
	glGenBuffers(1, &objects.VBO);
	glBindVertexArray(objects.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, objects.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);      
}

void font::gl_text_mode()
{
	//@Note: this now happens in the init_font function.
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    graphics::set_shader(graphics::Shader_Type::SHADER_TEXT); 
}

void font::draw_text(std::string& text, /*Font font, */ uint32_t start_x, uint32_t start_y, float scale, Vec3 color)//, Text_Effect effect)
{
	font::gl_text_mode();
    font::gl_Objects& gl_font = font::gl_objects();

	glUniform3f(glGetUniformLocation(graphics::shaders().text, "text_color"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(gl_font.VAO);   
    //@Note: this should match the Active Texture (i.e. GL_TEXTURE0). we're lucky that it does right now.
    //glUniform1i(glGetUniformLocation(graphics::shaders().text, "text"), 0);

    auto settings  = graphics::window_settings();
    glViewport(0, 0,static_cast<int>(settings.width),static_cast<int>(settings.height));

	float top   = settings.height; // viewport 
	float bot   = 0.0f;
	float left  = 0.0f;
	float right = settings.width; // viewport
	float near_  = 0.0f;
	float far_   = 1.0f; // near and far are reserved by windows???

	// this should be transposed, but we need to decide how we do matrices.
	float projectionmatrix[16] =   {2.0f / (right - left),   0.0f,                 0.0f,                 - (right + left) / (right - left),
								    0.0f,            2.0f / (top - bot),           0.0f,                   - (top + bot) / (top - bot),
									0.0f,                   0.0f,          (-2.0f / (far_ - near_)),     -(far_ + near_) / (far_ - near_),
									0.0f,                   0.0f,                   0.0f,                    1};

    //@refactor: this projection matrix does not ever change.
    // do we want to store this somewhere?
    Mat4 projection_matrix = mmat::ortho(left, right, top, bot, near_, far_);
    glUniformMatrix4fv(glGetUniformLocation(graphics::shaders().text, "projection"), 1, true, &projectionmatrix[0]);
    
	const auto& character_map = font::characters();
	
    // Iterate through all characters
    for (const auto& single_char: text)
    {

        //@Log: character_not_found_error?
        // auto search = character_map.find(single_char);
        // if (search == character_map.end())
        // 	fmt::print("single_char not found. character is {}\n", single_char);

        auto& char_glyph = character_map[single_char]; //Character&?

        GLfloat xpos = start_x + char_glyph.bearing.x * scale;
        GLfloat ypos = start_y - (char_glyph.size.y - char_glyph.bearing.y) * scale;

        GLfloat width  = char_glyph.size.x * scale;
        GLfloat height = char_glyph.size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] =
        {
            { xpos,     ypos + height,   0.0, 0.0 },            
            { xpos,          ypos,       0.0, 1.0 },
            { xpos + width,  ypos,       1.0, 1.0 },

            { xpos,         ypos + height,   0.0, 0.0 },
            { xpos + width,      ypos,       1.0, 1.0 },
            { xpos + width, ypos + height,   1.0, 0.0 }           
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, char_glyph.textureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gl_font.VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        start_x += (char_glyph.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);   
}


