#ifndef INCLUDED_INPUT_
#define INCLUDED_INPUT_
#include <vector>
#include "../vec2/vec2.h"
enum class Input_Event
{
    KEYBOARD,
    MOUSE
};



namespace input
{
    enum Key_Input : uint32_t
    {
        KEY_W,
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_UP,
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT,
        MOUSE_LEFT,
        MOUSE_RIGHT
    };
    
    std::vector<Key_Input>& input_queue();
    Vec2i& new_mouse_coordinates();
}

#endif