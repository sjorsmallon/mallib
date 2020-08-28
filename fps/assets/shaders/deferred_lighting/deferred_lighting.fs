#version 330 core

// defines
const int NUM_LIGHTS = 32;
const float ambient_component = 0.1f;

out vec4 fragment_color;
in vec2 texture_coords;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo_spec;

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light lights[NUM_LIGHTS];
uniform vec3 view_position;

void main()
{             
    // retrieve data from G-buffer
    vec3 fragment_position = texture(g_position, texture_coords).rgb;

    vec3 normal = texture(g_normal, texture_coords).rgb;
    vec3 albedo = texture(g_albedo_spec, texture_coords).rgb;
    float specular = texture(g_albedo_spec, texture_coords).a;
    
    // then calculate lighting as usual
    vec3 lighting = albedo * ambient_component;
    vec3 view_direction = normalize(view_position - fragment_position);

    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 light_direction = normalize(lights[i].Position - fragment_position);
        vec3 diffuse = max(dot(normal, light_direction), 0.0) * Albedo * lights[i].Color;
        lighting += diffuse;
    }
    
    fragment_color = vec4(lighting, 1.0);
}  