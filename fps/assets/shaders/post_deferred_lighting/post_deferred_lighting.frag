#version 430 core

// defines
#define LIGHT_BUFFER_BINDING_IDX 2
#define bool32 unsigned int

//@VOLATILE: this should match the number of lights in the buffer.
const int NUM_LIGHTS = 32;
const float ambient_component = 0.50f;

out vec4 fragment_color;

in vec2 texture_coords;

// framebuffers
uniform sampler2D fb_position;
uniform sampler2D fb_normal;
uniform sampler2D fb_albedo_spec;

uniform vec4 view_position;



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
    Light lights[NUM_LIGHTS];
};


void main()
{             
    // retrieve data from G-buffer
    vec3 fragment_position = texture(fb_position, texture_coords).rgb;

    vec3 normal = texture(fb_normal, texture_coords).rgb;
    vec3 albedo = texture(fb_albedo_spec, texture_coords).rgb;
    float specular = texture(fb_albedo_spec, texture_coords).a;
    
    // then calculate lighting as usual
    vec3 lighting = albedo * ambient_component;
    vec3 view_direction = normalize(fragment_position - view_position.xyz);

    for (int light_idx = 0; light_idx < NUM_LIGHTS; ++light_idx)
    {
        // if (lights[light_idx].on > 0)
        // if (true)
        // {
            float distance = distance(lights[light_idx].position.xyz, fragment_position);
            // if (distance < lights[light_idx].radius)
            // {
                //world position
                // diffuse 
                vec3 light_direction = normalize(lights[light_idx].position.xyz - fragment_position);
                vec3 diffuse = max(dot(normal, light_direction), 0.0) * albedo * lights[light_idx].color.xyz;

                // specular
                vec3 half_way_direction = normalize(light_direction + view_direction);
                float specular_local = pow(max(dot(normal, half_way_direction), 0.0), 32.0);
                vec3 specular_contribution = lights[light_idx].color.xyz * specular_local * specular;

                // float attenuation = 1.0f;
                
                float attenuation = 1.0 / (1.0 + lights[light_idx].linear * distance + lights[light_idx].quadratic * distance * distance);

                diffuse  *= attenuation;
                specular_contribution *= attenuation;
                lighting +=  diffuse + specular_contribution; 
            // }
        // }
    }

    fragment_color = vec4(lighting, 1.0);
}  