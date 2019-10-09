#include "font.h"

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <iostream>
#include <string>
#include "../graphics/gl_lite.h"
#include "../graphics/graphics.h"

void font::startup()
{
	glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const auto& settings  = graphics::window_settings();
    glViewport(0, 0,static_cast<int>(settings.width),static_cast<int>(settings.height));

	float top   = settings.height; // viewport 
	float bot   = 0.0f;
	float left  = 0.0f;
	float right = settings.width; // viewport
	float near_  = 0.0f;
	float far_   = 1.0f; // near and far are reserved by windows???
	
    glUseProgram(graphics::shaders().text);
	float projectionmatrix[16] =   {2.0f / (right - left),   0.0f,                 0.0f,                 - (right + left) / (right - left),
								    0.0f,            2.0f / (top - bot),           0.0f,                   - (top + bot) / (top - bot),
									0.0f,                   0.0f,          (-2.0f / (far_ - near_)),     -(far_ + near_) / (far_ - near_),
									0.0f,                   0.0f,                   0.0f,                    1};

    glUniformMatrix4fv(glGetUniformLocation(graphics::shaders().text, "projection"), 1, GL_TRUE, &projectionmatrix[0]);

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "../fonts/arial.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        uint32_t texture;
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
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character{
            texture,
           	{face->glyph->bitmap.width, face->glyph->bitmap.rows}, //uints
            {face->glyph->bitmap_left, face->glyph->bitmap_top}, //ints
            face->glyph->advance.x
        };

       font::characters().insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    
    // Configure VAO/VBO for texture quads
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

    // Game loop
    while (true)
    {

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RenderText("This is sample text", 25.0f, 25.0f, 1.0f, Vec3{0.5, 0.8f, 0.2f});
        RenderText("(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, Vec3{0.3, 0.7f, 0.9f});
       
        // Swap the buffers
        graphics::swap_buffers();
    }

}


void font::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, Vec3 color)
{
    // Activate corresponding render state	
    // shader.Use();
    graphics::set_shader(graphics::Shader_Type::SHADER_TEXT); 
    GLuint text_program =  graphics::shaders().text;
	font::gl_Objects& objects = font::gl_objects();
    GLuint VAO = objects.VAO;
    GLuint VBO = objects.VBO;
    glUniform3f(glGetUniformLocation(text_program, "text_color"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = font::characters()[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}