#version 450 core

//-------------------
// Defines
//-------------------

// order:
// position,
// normal
// diffuse / specular,
// roughness
// metallic
// ambient_occlusion
// displacement

//-------------------
// Outputs
//-------------------
layout (location = 0) out vec3 fb_position;
layout (location = 1) out vec3 fb_normal;
layout (location = 2) out vec4 fb_albedo_spec;
layout (location = 3) out vec3 fb_roughness;
layout (location = 4) out vec3 fb_metallic;
layout (location = 5) out vec3 fb_ambient_occlusion;
layout (location = 6) out vec3 fb_displacement;
layout (location = 7) out vec3 fb_tnormal;

//-------------------
// Inputs
//-------------------
layout (location = 0) in vec3 fragment_position;
layout (location = 1) in vec3 fragment_normal;
layout (location = 2) in vec3 view_position;
layout (location = 3) in vec3 light_position;
layout (location = 4) in vec2 texture_coords;



//-------------------
// uniforms
//-------------------
uniform sampler2D texture_albedo;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metallic;
uniform sampler2D texture_ambient_occlusion;
uniform sampler2D texture_displacement;

//-------------------
// Implementation
//-------------------

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    fb_position = fragment_position;

    // also store the per-fragment normals into the gbuffer
    fb_normal = normalize(fragment_normal);

    // and the diffuse per-fragment color ("albedo")
    fb_albedo_spec.rgb = texture(texture_albedo, texture_coords).rgb;
    fb_albedo_spec.a = 16.0f;

    // store PBR related things.
    fb_roughness = normalize(view_position);
    fb_metallic = normalize(light_position);
    fb_ambient_occlusion = texture(texture_ambient_occlusion, texture_coords).rgb;
    
    fb_displacement = texture(texture_displacement, texture_coords).rgb;
    fb_tnormal = texture(texture_normal, texture_coords).rgb;

}  


