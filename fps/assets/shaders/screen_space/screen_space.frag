#version 450 core

out vec4 fragment_color;

in vec2 texture_coords;

// texture
uniform sampler2D hud_texture;

void main()
{             
    vec4 texture_color = texture(hud_texture, texture_coords).rgba;
    fragment_color = texture_color;
}  