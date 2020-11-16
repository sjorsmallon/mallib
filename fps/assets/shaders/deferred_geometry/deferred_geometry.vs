#version 430 core

layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 texture_coords_VS_in;

layout (location = 0) out vec3 fragment_position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec2 texture_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 world_position = model * vec4(position_VS_in, 1.0);
    fragment_position = world_position.xyz; 
    texture_coords = texture_coords_VS_in;
    
    mat3 normal_matrix = transpose(inverse(mat3(view * model)));
    normal = normal_matrix * normal_VS_in;

    gl_Position = projection * view * world_position;
}