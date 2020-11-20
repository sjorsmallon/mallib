#version 430 core
layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 texture_coords_VS_in;

uniform mat4 light_space_matrix;
uniform mat4 model;

void main()
{
    gl_Position = light_space_matrix * model * vec4(position_VS_in, 1.0);
}  