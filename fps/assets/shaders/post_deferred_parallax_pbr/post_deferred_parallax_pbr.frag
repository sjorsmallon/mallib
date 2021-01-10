#version 450 core

//@Note(Sjors): the Normal that is used here is not the default normal (i.e. normal_matrix * inverse(tranpose(model))),
// but a normal in a different space (model_matrix * normal). I don't know why.


//-------------------
// Defines
//-------------------
#define LIGHT_BUFFER_BINDING_IDX 2
#define bool32 unsigned int

const int NUM_LIGHTS = 32; //@VOLATILE: this should match the number of lights in the buffer.
const float PI = 3.14159265359;
const float height_scale = 0.1f;

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

// vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
// { 
//     float height =  texture(fb_displacement, texCoords).r;    
//     vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
//     return texCoords - p;    
// } 

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(fb_displacement, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(fb_displacement, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(fb_displacement, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}


mat3 get_TBN_matrix()
{
    vec3 normal    = texture(fb_normal, texture_coords).xyz; 
    vec3 tangent   = texture(fb_roughness, texture_coords).xyz;
    vec3 bitangent = texture(fb_metallic, texture_coords).xyz;

    mat3 TBN = mat3(tangent, bitangent, normal);

    return TBN;
}


const float gamma = 2.2f;

void main()
{
    vec3 tangent_fragment_position = texture(fb_position, texture_coords).xyz;
    vec3 normal = texture(fb_tnormal, texture_coords).xyz * 2.0 - 1.0;
    
    vec3 tangent_view_position = texture(fb_roughness, texture_coords).xyz;
    vec3 tangent_light_position = texture(fb_metallic, texture_coords).xyz;

    vec3 view_dir = normalize(tangent_view_position - tangent_fragment_position);
    vec2 new_tex_coords = ParallaxMapping(texture_coords,  view_dir);       

    if(new_tex_coords.x > 1.0 || new_tex_coords.y > 1.0 || new_tex_coords.x < 0.0 || new_tex_coords.y < 0.0)
    {
        fragment_color = vec4(1.0,0.0f,0.0f,1.0f);
        discard;
        return;
    }
   
    // get diffuse color
    vec3 color = texture(fb_albedo_spec, new_tex_coords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 light_direction = normalize(tangent_light_position - tangent_fragment_position);
    float diff = max(dot(light_direction, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular    
    vec3 reflectDir = reflect(-light_direction, normal);
    vec3 halfwayDir = normalize(light_direction + view_dir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    fragment_color = vec4(ambient + diffuse + specular, 1.0);

    
}

