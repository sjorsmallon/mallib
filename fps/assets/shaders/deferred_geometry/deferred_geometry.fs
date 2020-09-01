#version 430 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo_spec;

in vec2 texture_coords;
in vec3 fragment_position;
in vec3 normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    g_position = fragment_position;

    // also store the per-fragment normals into the gbuffer
    g_normal = normalize(normal);

    // and the diffuse per-fragment color
    g_albedo_spec.rgb = texture(texture_diffuse1, texture_coords).rgb;
    
    // store specular intensity in g_albedo_spec's alpha component
    g_albedo_spec.a = texture(texture_specular1, texture_coords).r;
}  