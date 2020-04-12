#version 430 core

//--- Inputs ---------------------------------------
layout (location = 0) in vec3 position_fs;
layout (location = 1) in vec2 texture_coordinates_fs;
layout (location = 2) in vec3 normal_fs;
layout (location = 3) in vec3 world_position_fs;

//--- Outputs --------------------------------------
layout (location = 0) out vec4 fragment_color;

//--- Uniforms -------------------------------------
layout (location = 9)  uniform vec3 light_position; // light is defined in world coordinates.
layout (location = 10) uniform vec3 light_color;
layout (location = 11) uniform vec4 material; // , x = kd, y = ks, z = n, w = ka (glossiness).

void main()
{
    // using blinn-phong with half normal.

    vec3 world_position_light = light_position.xyz;
    //vec3 world_position_light = vec3(50.0f, 100.0f, 100.0f);

    vec3 L = normalize(world_position_light.xyz - position_fs);
    vec3 N = normalize(normal_fs);
    vec3 R = normalize(reflect(L, N));

    // camera position. assume at the origin for now.
    vec3 V = vec3(0.0f,0.0f,1.0f); 

    V = normalize(V - world_position_fs);

    // half vector for blinn-phong.
    vec3 H = normalize(L + V);
 
    vec3 material_color = vec3(100.0f/255.0f, 149.0f/255.0f, 237/255.0f);

    float NdotL = dot(N,L);
    float NdotH = dot(N,H);


     float light_intensity = NdotL > 0.0f ? 1.0f : 0.0f;

    light_intensity = smoothstep(0.0f, 0.01f, NdotL);
    vec3 ambient_light_color = vec3(0.1f, 0.1f, 0.5f);

    vec3 ambient_component = ambient_light_color;
    vec3 diffuse_component = material_color;
    vec3 lighting_component = light_intensity * light_color;
    float glossiness = material.w;
    float specular_intensity = pow(NdotH * light_intensity, glossiness * glossiness);
    float smooth_specular_intensity = smoothstep(0.0005f, 0.01f, specular_intensity);
    float specular_component = smooth_specular_intensity;

    // Rim Lighting
    vec3 rim_color = vec3(0.8f,0.8f,0.8f);
    float rim_dot = 1 - dot(V, N);
    float rim_amount = 0.716f;
    float rim_threshold = 0.1f;
    float rim_intensity = rim_dot * pow(NdotL, rim_threshold); 

    vec3 rim_component =  rim_intensity * rim_amount * rim_color;

    vec4 white = vec4(1.0f,1.0f,1.0f, 1.0f);

    // fragment_color = vec4(material_color * (ambient_component + lighting_component + specular_component + rim_component), 1.0f);
    fragment_color = vec4(lighting_component + specular_component + rim_component  + ambient_component, 1.0f) * white;

}
