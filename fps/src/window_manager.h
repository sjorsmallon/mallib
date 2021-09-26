#ifndef INCLUDED_WINDOW_MANAGER_
#define INCLUDED_WINDOW_MANAGER_
#include "logr.h"
#include "input.h"

//@FIXME(Sjors): I hate that input is part of the window manager now. where do we want to keep it around?
// Since glfw works with callbacks, we need some way to access the scope 

//forward decl.
struct GLFWwindow;


//@Note: Inits glfw.
struct Window_Manager
{
	GLFWwindow*  main_window = nullptr;
	GLFWwindow*  debug_window = nullptr;
	Input input{};

	Window_Manager();
	~Window_Manager();

};

void create_main_window(Window_Manager& window_manager, const char* title, const int window_width, const int window_height);
void poll_input(Window_Manager& window_manager);
void swap_buffers(const Window_Manager& window_manager);
// void render_debug_ui(const Window_Manager& window_manager);


#endif