#ifndef INCLUDED_WINDOW_MANAGER_
#define INCLUDED_WINDOW_MANAGER_
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// once
GLFWwindow* glfw_init_window(const char* title, const int window_width, const int window_height);
GLFWwindow* glfw_init_imgui_window(const char* title, const int window_width, const int window_height, GLFWwindow* parent_window);
void 		glfw_exit_and_cleanup(GLFWwindow* window);



#endif