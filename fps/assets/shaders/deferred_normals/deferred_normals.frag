#version 430 core

// defines
#define LIGHT_BUFFER_BINDING_IDX 2
const int NUM_LIGHTS = 32;
const float ambient_component = 0.05f;

out vec4 fragment_color;

in vec2 texture_coords;

uniform sampler2D fb_position;
uniform sampler2D fb_normal;
uniform sampler2D fb_albedo_spec;

struct Light {
    vec4 position;
    vec4 color;
};

layout (std140, binding = LIGHT_BUFFER_BINDING_IDX) uniform Lights
{
    Light lights[32];
};

uniform vec3 view_position;

void main()
{             
    // retrieve data from G-buffer
    vec3 fragment_position = texture(fb_position, texture_coords).rgb;

    vec3 normal = texture(fb_normal, texture_coords).rgb;
    normal = (0.5f * normalize(normal)) + 0.5f;
    fragment_color = vec4(normal, 1.0f);
}  