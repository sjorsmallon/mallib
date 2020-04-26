#version 430
// Fragment shader

layout (location = 0) in vec4 vertex_position_FS_in;
layout (location = 1) in vec4 vertex_color_FS_in;


layout(location = 0) out fragment_color

void main()
{
	fragment_color = vertex_color_FS_in;
}