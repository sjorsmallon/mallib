#version 430 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 tex_coords;
layout (location = 0) uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * vec4(vertex.xy, 0.0, 1.0);
    tex_coords = vertex.zw;
}  