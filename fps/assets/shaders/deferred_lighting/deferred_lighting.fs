#version 430 core

// defines
#define LIGHT_BUFFER_BINDING_IDX 2

#define bool32 unsigned int

//@VOLATILE: this should match the number of lights in the buffer.
const int NUM_LIGHTS = 32;
const float ambient_component = 0.20f;


out vec4 fragment_color;

in vec2 texture_coords;

uniform sampler2D fb_position;
uniform sampler2D fb_normal;
uniform sampler2D fb_albedo_spec;

struct Light {
    vec4 position;
    vec4 color;
    bool32 on;
    float linear;
    float quadratic;
    float radius;
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
    vec3 albedo = texture(fb_albedo_spec, texture_coords).rgb;
    float specular = texture(fb_albedo_spec, texture_coords).a;
    
    // then calculate lighting as usual
    vec3 lighting = albedo * ambient_component;
    vec3 view_direction = normalize(view_position - fragment_position);

    for (int light_idx = 0; light_idx < NUM_LIGHTS; ++light_idx)
    {
        if (lights[light_idx].on > 0)
        {
            float distance = distance(lights[light_idx].position.xyz,fragment_position);
            // diffuse 
            vec3 light_direction = normalize(lights[light_idx].position.xyz - fragment_position);
            vec3 diffuse = max(dot(normal, light_direction), 0.0) * albedo * lights[light_idx].color.xyz;

            float attenuation = 1.0 / (1.0 + lights[light_idx].linear * distance + lights[light_idx].quadratic * distance * distance);

            lighting += attenuation * diffuse;    
        }
        
    }
    
    fragment_color = vec4(lighting, 1.0);
}  