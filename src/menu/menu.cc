#include "menu.h"
#include "../font/font.h"
#include "../graphics/graphics.h"

void menu::draw_menu()
{
	// graphics::render_2d_left_handed_unit();
 	const auto& window_settings = graphics::window_settings(); // also render target?

 	float vertical_offset = 200.0f;
 	float vertical_stride = 100.0f;
 	float center_x = window_settings.width  / 2.0f;
 	float cursor_y = (window_settings.height - vertical_offset);

 	std::string text = "Start game";
    Vec3 color;
    color.r = 0.5f;
    color.g = 0.8f;
    color.b = 0.7f;
    float scale = 1.0f;

    //@TODO:"how to deal with selected thing flashing in other color? think about that.
    std::vector<font::Character> font;
    // generate_font_at_size(font_name, height, color);


    //
    // Menu Item: Start Game
    //
    font::draw_text("Start game", center_x, cursor_y, scale, color);//, Text_Effect effect);
    //
    // Menu Item: Exit
    //
    cursor_y -= vertical_stride;
    Vec3 exit_color = color;
    font::draw_text("Exit" , center_x, cursor_y, 2.0f, exit_color);
}
