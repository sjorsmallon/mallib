#version 430 core

// Specify the input locations of attributes
layout (location = 0) in vec4 vertex_position_in;
layout (location = 1) in vec4 vertex_color_in;

layout (location = 0) out vec4 vertex_position_FS_in;
layout (location = 1) out vec4 vertex_color_FS_in;

void main() 
{
	gl_Position = vertex_position_in;
	vertex_position_FS_in = vertex_position_in;
	vertex_color_FS_in = vertex_color_in;


}