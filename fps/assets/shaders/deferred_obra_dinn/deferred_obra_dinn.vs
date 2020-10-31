#version 430 core
layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec2 texture_coords_VS_in;

out vec2 texture_coords;

void main()
{
    gl_Position = vec4(position_VS_in, 1.0);
    texture_coords = texture_coords_VS_in;
}