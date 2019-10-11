#ifndef INCLUDED_INPUT_
#define INCLUDED_INPUT_
#include <vector>

enum class Input_Event
{
    KEYBOARD,
    MOUSE
};



namespace input
{
    enum class Key_Input
    {
        KEY_W,
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_UP,
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT
    };
    
    std::vector<Key_Input>& input_queue();
}

#endif