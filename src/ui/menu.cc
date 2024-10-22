#include <menu.h>

#include "fmt/core.h"

#include <font.h>
#include <graphics.h>
#include <globals.h> // screen dimensions.

font::Font& menu::menu_font()
{
    static font::Font menu_font;
    return menu_font;
}

void menu::init_menu()
{
    const uint32_t font_height = uint32_t{50}; // in pixels
    font::generate_font_at_size(menu::menu_font(), "assets/fonts/opensans.ttf", font_height);
}

menu::Menu_Item& menu::active_start_menu_item()
{
    static menu::Menu_Item active_start_menu_item = menu::Menu_Item::NONE;
    return active_start_menu_item;
}

void menu::draw_menu()
{
    // selected_menu_item
	// graphics::render_2d_left_handed_unit();
    // glViewport?
    const uint32_t window_width = globals.window_width;
    const uint32_t window_height = globals.window_height;

 	float vertical_offset = 200.0f;
 	float vertical_stride = 100.0f;
 	float center_x = window_width  / 2.0f;
 	float cursor_y = (window_height - vertical_offset);

    // retrieve the menu font.
    auto& menu_font = menu::menu_font();

    const float scale = 1.0f; // we shouldn't ever scale font since we can just request the wanted size (otherwise we get stretching)
    const mgl::vec3 default_color  = {0.19f, 0.717f,0.17f};
    const mgl::vec3 selected_color = {0.12f, 0.76f, 0.717f};
    mgl::vec3 start_game_color = default_color;
    mgl::vec3 settings_color   = default_color;
    mgl::vec3 exit_game_color  = default_color;

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
        std::string start_game{"Start game"};
        uint32_t start_game_width = font::get_string_width_in_pixels(start_game, menu_font);
        uint32_t start_game_x = center_x - 0.5f * start_game_width;
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
        std::string settings{"Settings"};
        uint32_t settings_width = font::get_string_width_in_pixels(settings, menu_font);
        uint32_t settings_x = center_x - 0.5f * settings_width;
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
        std::string exit{"Exit"};
        uint32_t exit_width = font::get_string_width_in_pixels(exit, menu_font);
        uint32_t exit_x = center_x - 0.5f * exit_width;
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
