#ifndef INCLUDED_INPUT_MANAGER_
#define INCLUDED_INPUT_MANAGER_

//@NOTE(Sjors): For now, this maps linearly to the keymap used in GLFW.
struct Input
{
	// keyboard
	bool keyboard_state[256];
	bool prev_keyboard_state[256];


	// mouse
	bool mouse_left;
	bool mouse_right;

	bool prev_mouse_left;
	bool prev_mouse_right;

	double mouse_x;
	double mouse_y;

	double mouse_delta_x;
	double mouse_delta_y;
	// bool f_keys_pressed[13]; //@NOTE: f_keys map at one offset (i.e. F1 -> f_keys_pressed[1])

	double mouse_scroll_delta_y;
	double mouse_scroll_delta_x;
};





// #include <stdint.h>
// #include <functional>
// #include <map>

// using key_code_t = int32_t;
// using command_t = std::function<void()>;


// struct Input_Manager
// {
// 	// std::map<key_code_t, Key> keys;
// 	Input_Manager() = default;
// 	~Input_Manager() = default;
// };

// this binding can be satisfied somewhere else.

// std::map<key_code_t, command_t> commands;
// void register_command(Input_Manager& manager, key_code_t key_code, command_t command);
// auto input_manager = Input_Manager();
// register_command(input_manager,  KEY_W, std::move([](){
//     logr::report("test.\n");
// }));



#endif

