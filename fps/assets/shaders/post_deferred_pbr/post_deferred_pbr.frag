#version 430 core

//-------------------
// Defines
//-------------------
#define LIGHT_BUFFER_BINDING_IDX 2
#define bool32 unsigned int

const int NUM_LIGHTS = 32; //@VOLATILE: this should match the number of lights in the buffer.
const float ambient_component = 0.50f;
const float PI = 3.14159265359;


//-------------------
// Outputs
//-------------------
out vec4 fragment_color;

//-------------------
// Inputs
//-------------------
in vec2 texture_coords;


//-------------------
// Uniforms
//-------------------
// framebuffers
uniform sampler2D fb_position;
uniform sampler2D fb_normal;
uniform sampler2D fb_albedo_spec;
uniform sampler2D fb_roughness;
uniform sampler2D fb_metallic;
uniform sampler2D fb_ambient_occlusion;
uniform sampler2D fb_displacement;
uniform sampler2D fb_tnormal;

uniform vec4 view_position;


//---------------------------
// Structure Definitions
//---------------------------

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

//-------------------
// Implementation
//-------------------

float distribution_GGX(vec3 N, vec3 H, float a)
{
    float a_squared = a * a;
    float n_dot_h  = max(dot(N, H), 0.0f);
    float ndh_squared = n_dot_h * n_dot_h;

    float denom  = (ndh_squared * (a_squared - 1.0f) + 1.0f);
    denom        = PI * denom * denom;
    
    return a_squared / denom;
}

float geometry_schlick_GGX(float n_dot_v, float k)
{
    
    float denom = n_dot_v * (1.0f - k) + k;
    
    return n_dot_v / denom;
}
  
float geometry_smith(vec3 N, vec3 V, vec3 L, float k)
{
    float n_dot_v = max(dot(N, V), 0.0);
    float n_dot_l = max(dot(N, L), 0.0);
    float GGX_1 = geometry_schlick_GGX(n_dot_v, k);
    float GGX_2 = geometry_schlick_GGX(n_dot_l, k);
    
    return GGX_1 * GGX_2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 get_tangent_normals_to_world_space()
{
    vec3 tangent_texture_normal = texture(fb_tnormal, texture_coords).xyz * 2.0 - 1.0;
    vec3 fragment_normal = texture(fb_normal, texture_coords).rgb;
    vec3 fragment_position = texture(fb_position, texture_coords).rgb;

    vec3 Q1  = dFdx(fragment_position);
    vec3 Q2  = dFdy(fragment_position);
    vec2 st1 = dFdx(texture_coords);
    vec2 st2 = dFdy(texture_coords);

    vec3 N   = normalize(fragment_normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_texture_normal);
}



// ----------------------------------------------------------------------------
void main()
{       
    vec3 albedo = texture(fb_albedo_spec, texture_coords).rgb;
    float metallic  = texture(fb_metallic, texture_coords).r;
    float roughness = texture(fb_roughness, texture_coords).r;
    float ambient_occlusion = texture(fb_ambient_occlusion, texture_coords).r;

    vec3 world_position = texture(fb_position, texture_coords).rgb;

    vec3 N = get_tangent_normals_to_world_space();
    vec3 V = normalize(view_position.xyz - world_position.xyz);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    // F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int idx = 0; idx < NUM_LIGHTS; ++idx) 
    {

        // calculate per-light radiance
        vec3 L = normalize(lights[idx].position.xyz - world_position.xyz);
        vec3 H = normalize(V + L);
        float distance = length(lights[idx].position.xyz - world_position.xyz);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = vec3(lights[idx].color * attenuation);

        // Cook-Torrance BRDF
        float NDF = distribution_GGX(N, H, roughness);   
        float G   = geometry_smith(N, V, L, roughness);      
        vec3 F    = fresnel_schlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;     

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ambient_occlusion;

    vec3 color = ambient + Lo;

    // HDR tonemapping
   color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    fragment_color = vec4(color, 1.0);
}

