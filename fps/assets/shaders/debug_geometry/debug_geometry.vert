#version 450 core

layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec3 color_VS_in;

layout (location = 0) out vec3 color_base;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 world_position = model * vec4(position_VS_in, 1.0);
    gl_Position = projection * view * world_position;
    color_base = color_VS_in;
}