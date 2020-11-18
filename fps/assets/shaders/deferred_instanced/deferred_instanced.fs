#version 430 core

layout (location = 0) out vec3 fb_position;
layout (location = 1) out vec3 fb_normal;
layout (location = 2) out vec4 fb_albedo_spec;

layout (location = 0) in vec3 fragment_position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coords;
in flat int instance_ID;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    fb_position = fragment_position;

    // also store the per-fragment normals into the gbuffer
    fb_normal = normalize(normal);

    // and the diffuse per-fragment color
    fb_albedo_spec.rgb = texture(texture_diffuse, texture_coords).rgb;
    
    // store specular intensity in fb_albedo_spec's alpha component
    fb_albedo_spec.a = texture(texture_specular, texture_coords).r;
}  