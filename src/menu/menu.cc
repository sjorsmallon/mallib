#include "menu.h"
#include "../font/font.h"
#include "../graphics/graphics.h"



void menu::draw_menu()
{
 	const auto& window_settings = graphics::window_settings(); // also render target?

 	float vertical_offset = 200.0f;
 	float center_x = window_settings.width  / 2.0f;
 	float start_y = (window_settings.height - vertical_offset);

 	std::string text = "Start game";
    Vec3 color;
    color.r = 0.5f;
    color.g = 0.8f;
    color.b = 0.7f;
    float scale = 1.0f;

    //@TODO:"how to deal with selected thing flashing in other color? think about that.
    // font = generate_font_at_size(font name, height, color?);
    font::draw_text(text, center_x, start_y, scale, color);//, Text_Effect effect);

	text = "Exit";
    font::draw_text(text, center_x, start_y - 48, 2.0f, {0.3f, 0.2f, 0.4f});
}
