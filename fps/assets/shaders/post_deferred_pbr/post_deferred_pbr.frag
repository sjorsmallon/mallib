#version 450 core

//@Note(Sjors): the Normal that is used here is not the default normal (i.e. normal_matrix * inverse(tranpose(model))),
// but a normal in a different space (model_matrix * normal). I don't know why.

//@NOTE(SJORS): AMBIENT & METALLIC ARE DISABLED!


//-------------------
// Defines
//-------------------
#define LIGHT_BUFFER_BINDING_IDX 2
#define bool32 unsigned int

const int NUM_LIGHTS = 32; //@VOLATILE: this should match the number of lights in the buffer.
const float PI = 3.14159265359;
const float height_scale = 12.0f;

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

float distribution_GGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a_squared = a * a;
    float n_dot_h  = max(dot(N, H), 0.0f);
    float n_dot_h_squared = n_dot_h * n_dot_h;

    float nom = a_squared;
    float denom  = (n_dot_h_squared * (a_squared - 1.0f) + 1.0f);
    denom        = PI * denom * denom;
    
    return nom / denom;
}

float geometry_schlick_GGX(float n_dot_v, float roughness)
{
    float r = (roughness + 1.0);
    float k  = (r * r) / 8.0;

    float nom = n_dot_v;
    float denom = n_dot_v * (1.0f - k) + k;
    
    return nom / denom;
}
  
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float n_dot_v = max(dot(N, V), 0.0);
    float n_dot_l = max(dot(N, L), 0.0);
    float GGX_2 = geometry_schlick_GGX(n_dot_v, roughness);
    float GGX_1 = geometry_schlick_GGX(n_dot_l, roughness);
    
    return GGX_1 * GGX_2;
}

vec3 fresnel_schlick(float cos_theta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cos_theta, 0.0), 5.0);
}

mat3 get_TBN_matrix()
{
    vec3 fragment_normal = texture(fb_normal, texture_coords).xyz;
    vec3 fragment_position = texture(fb_position, texture_coords).xyz;

    vec3 Q1  = dFdx(fragment_position);
    vec3 Q2  = dFdy(fragment_position);
    vec2 st1 = dFdx(texture_coords);
    vec2 st2 = dFdy(texture_coords);

    vec3 N  = normalize(fragment_normal);
    vec3 T = st2.y * Q1 - st1.y * Q2;
    vec3 B = st1.x * Q2 - st2.x * Q1;
    T = T - N * dot(T, N);
    B = B - N * dot(N, N);
    B = B - T * dot(B, T);
  
    mat3 TBN = mat3(T, B, N);

    return TBN;
}

const float gamma = 2.2f;
const float ambient_light_amount = 0.5f;

void main() 
{
    vec3 albedo = pow(texture(fb_albedo_spec, texture_coords).rgb, vec3(gamma));
    float metallic  = texture(fb_metallic, texture_coords).r;
    // float metallic = 0.0f;
    float roughness = texture(fb_roughness, texture_coords).r;
    float ambient_occlusion = texture(fb_ambient_occlusion, texture_coords).r;
    vec3 world_position = texture(fb_position, texture_coords).rgb;
    mat3 TBN = get_TBN_matrix();

    vec3 normal = texture(fb_tnormal, texture_coords).xyz * 2.0 - 1.0;
    normal = normalize(TBN * normal);
    vec3 N = normal;

    vec3 V = normalize(view_position.xyz - world_position.xyz);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    // F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int idx = 0; idx < 1; ++idx) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lights[idx].position.xyz - world_position.xyz);
        float distance = length(lights[idx].position.xyz - world_position.xyz);

        vec3 H = normalize(V + L);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = vec3(lights[idx].color.xyz * 255.0 * attenuation);

        // Cook-Torrance BRDF
        float NDF = distribution_GGX(N, H, roughness);   
        float G   = geometry_smith(N, V, L, roughness);      
        vec3 F    = fresnel_schlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
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
    vec3 ambient = vec3(ambient_light_amount) * albedo * ambient_occlusion;


    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/gamma)); 

    fragment_color = vec4(color, 1.0);
}

