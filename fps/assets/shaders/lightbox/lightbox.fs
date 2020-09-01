#version 330 core
layout (location = 0) out vec4 fragment_color;

// could be vec3
uniform vec4 light_color;

void main()
{           
    fragment_color = vec4(light_color.xyz, 1.0);
}