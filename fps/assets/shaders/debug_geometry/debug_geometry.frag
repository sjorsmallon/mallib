#version 450 core
layout (location = 0) out vec4 fragment_color;


layout(location = 0) in vec3 color_base;

void main()
{           
    fragment_color = vec4(color_base, 1.0);
}