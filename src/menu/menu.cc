#include "menu.h"
#include "../font/font.h"
#include "../graphics/graphics.h"
#include "fmt/core.h"

menu::Menu_Item& menu::active_start_menu_item()
{
    static menu::Menu_Item active_start_menu_item = menu::Menu_Item::NONE;
    return active_start_menu_item;
}

void menu::draw_menu()
{
    // selected_menu_item
	// graphics::render_2d_left_handed_unit();
 	const auto& window_settings = graphics::window_settings(); // also render target?

 	float vertical_offset = 200.0f;
 	float vertical_stride = 100.0f;
 	float center_x = window_settings.width  / 2.0f;
 	float cursor_y = (window_settings.height - vertical_offset);

    //@Refactor: use this?
    // struct Text_Layout
    // {
    //     int string_width;
    //     int pixel_height;
    //     float at_x;
    //     float at_y;
    // }

    const uint32_t font_height = 50; // in pixels
    const float scale = 1.0f;
    font::Font menu_font = {};    
    font::generate_font_at_size(menu_font, "assets/fonts/opensans.ttf", font_height);

    Vec3 default_color =  {0.19f, 0.717f,0.17f};
    Vec3 selected_color = {0.12f, 0.76f, 0.717f};
    Vec3 start_game_color = default_color;
    Vec3 settings_color   = default_color;
    Vec3 exit_game_color  = default_color;

    font::Text_Effect start_game_effect = font::Text_Effect::NONE;
    font::Text_Effect settings_effect   = font::Text_Effect::NONE;
    font::Text_Effect exit_game_effect  = font::Text_Effect::NONE;




    //@Refactor: make a struct for this?
    switch(menu::active_start_menu_item())
    {
        case Menu_Item::NONE:
        {
            break;
        }
        case Menu_Item::START:
        {
            start_game_color = selected_color;
            start_game_effect = font::Text_Effect::COLOUR_SHIFT;
            break;
        }
        case Menu_Item::SETTINGS:
        {
            settings_color = selected_color;
            settings_effect = font::Text_Effect::COLOUR_SHIFT;  
            break;
        }
        case Menu_Item::EXIT:
        {
            exit_game_color = selected_color;
            exit_game_effect = font::Text_Effect::COLOUR_SHIFT;
            break;
        }
    }        


    //
    // Menu Item: Start Game
    //
    {
        //if ()

        std::string start_game = "Start game";
        uint32_t start_game_width = font::get_string_width_in_pixels(start_game, menu_font);
        uint32_t start_game_x = center_x - 0.5 * start_game_width;
        font::draw_text(start_game,
                        menu_font,
                        start_game_x,
                        cursor_y,
                        scale,
                        start_game_color,
                        start_game_effect);
    }

    //
    // Menu Item: Settings
    //
    {
        std::string settings = "Settings";
        uint32_t settings_width = font::get_string_width_in_pixels(settings, menu_font);
        uint32_t settings_x = center_x - 0.5 * settings_width;
        cursor_y -= vertical_stride;
        font::draw_text(settings,
                        menu_font,
                        settings_x,
                        cursor_y,
                        scale,
                        settings_color,
                        settings_effect);
    }
    //
    // Menu Item: Exit
    //
    {
        std::string exit = "Exit";
        uint32_t exit_width = font::get_string_width_in_pixels(exit, menu_font);
        uint32_t exit_x = center_x - 0.5 * exit_width;
        cursor_y -= vertical_stride;
        font::draw_text(exit,
                        menu_font,
                        exit_x,
                        cursor_y,
                        scale,
                        exit_game_color,
                        exit_game_effect);
    }
}
