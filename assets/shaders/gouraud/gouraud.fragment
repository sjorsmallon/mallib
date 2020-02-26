#version 430 core


layout (location = 0) in vec3 vertex_color;
layout (location = 1) in vec2 texture_coordinates;

layout (location = 0) uniform sampler2D texture_uniform;

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
layout (location = 0)out vec4 fColor;

void main()
{
    vec4 texture_color = texture2D(texture_uniform, texture_coordinates);

    fColor =  vec4(vertex_color, 1) * texture_color;
}