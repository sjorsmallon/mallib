#include "menu.h"
#include "../font/font.h"
#include "../graphics/graphics.h"
#include "fmt/core.h"

void menu::draw_menu()
{
	// graphics::render_2d_left_handed_unit();
 	const auto& window_settings = graphics::window_settings(); // also render target?

 	float vertical_offset = 200.0f;
 	float vertical_stride = 100.0f;
 	float center_x = window_settings.width  / 2.0f;
 	float cursor_y = (window_settings.height - vertical_offset);

    //@Refactor: font_height is not stored in the font now.
    uint32_t font_height = 50; 
    font::Font menu_font;    
    font::generate_font_at_size(menu_font, "../fonts/opensans.ttf", font_height);

    Vec3 start_game_color;
    start_game_color.r = 0.5f;
    start_game_color.g = 0.8f;
    start_game_color.b = 0.7f;
    float scale = 1.0f;

    //@TODO:"how to deal with selected thing flashing in other color? think about that.

    //
    // Menu Item: Start Game
    //
    std::string start_game = "Start game";
    uint32_t start_game_width = font::get_string_width_in_pixels(start_game, menu_font);
    uint32_t start_game_x = center_x - 0.5 * start_game_width;
    font::draw_text(start_game, menu_font, start_game_x, cursor_y, scale, start_game_color);//, Text_Effect effect);

    //
    // Menu Item: Settings
    //
    std::string settings = "Settings";
    uint32_t settings_width = font::get_string_width_in_pixels(settings, menu_font);
    uint32_t settings_x = center_x - 0.5 * settings_width;
    cursor_y -= vertical_stride;
    Vec3 settings_color = start_game_color; 
    font::draw_text(settings, menu_font, settings_x, cursor_y, scale, settings_color);//, Text_Effect effect);

    //
    // Menu Item: Exit
    //
    std::string exit = "Exit";
    uint32_t exit_width = font::get_string_width_in_pixels(exit, menu_font);
    uint32_t exit_x = center_x - 0.5 * exit_width;
    cursor_y -= vertical_stride;
    Vec3 exit_color = start_game_color;
    font::draw_text(exit, menu_font, exit_x, cursor_y, scale, exit_color);
}
