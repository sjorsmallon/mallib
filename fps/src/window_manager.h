#ifndef INCLUDED_WINDOW_MANAGER_
#define INCLUDED_WINDOW_MANAGER_
#include "log.h"
#include "input.h"

//@FIXME(Sjors): I hate that input is part of the window manager now. where do we want to keep it around?
// Since glfw works with callbacks, we need some way to access the scope 

//@NOTE(Sjors): the glfw window pointer is abused to record input.

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
void create_debug_window(Window_Manager& window_manager, const char* title, const int window_width, const int window_height);

// bool should_close_window(const Window_Manager& window_manager);

void poll_input(const Window_Manager& window_manager);
void swap_buffers(const Window_Manager& window_manager);


#endif