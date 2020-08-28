#ifndef INCLUDED_GLFW_WRAPPER_
#define INCLUDED_GLFW_WRAPPER_
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// once
GLFWwindow* glfw_init_main_window(const int window_width, const int window_height);
GLFWwindow* glfw_init_debug_window(const int debug_window_width, const int debug_window_height, GLFWwindow* parent_window);
void 		glfw_exit_and_cleanup(GLFWwindow* window);



#endif