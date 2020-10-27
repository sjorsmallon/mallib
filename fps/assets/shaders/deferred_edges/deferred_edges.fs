#version 430 core

// defines
#define LIGHT_BUFFER_BINDING_IDX 2
const int NUM_LIGHTS = 32;
const float ambient_component = 0.05f;

out vec4 fragment_color;

in vec2 texture_coords;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo_spec;


uniform vec3 view_position;

struct Light {
    vec4 position;
    vec4 color;
};

layout (std140, binding = LIGHT_BUFFER_BINDING_IDX) uniform Lights
{
    Light lights[32];
};

const float WINDOW_WIDTH = 1920.0f;
const float WINDOW_HEIGHT = 1080.0f;

const float offX = 1.0 / WINDOW_WIDTH;
const float offY = 1.0 / WINDOW_HEIGHT;

const float threadHold = 0.10;


bool sobel_albedo(vec2 texture_coords)
{
    vec3 c11 = texture(g_albedo_spec, texture_coords).xyz;
   
    float  s00 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2(-offX, -offY)).xyz) - threadHold);   
    float  s01 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2(   0.0f, -offY)).xyz)  - threadHold);
    float  s02 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2( offX, -offY)).xyz) - threadHold);

    float  s10 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2(-offX,   0.0f)).xyz) - threadHold);
    float  s12 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2( offX,   0.0f)).xyz) - threadHold);
   
    float  s20 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2(-offX, offY)).xyz) - threadHold);
    float  s21 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2(   0.0f,  offY)).xyz) - threadHold);
    float  s22 = max(0.0f, dot(c11, texture(g_albedo_spec, texture_coords + vec2( offX, offY)).xyz) - threadHold);
   
    float sobelX = s00 + 2.0f * s10 + s20 - s02 - 2.0f * s12 - s22;
    float sobelY = s00 + 2.0f * s01 + s02 - s20 - 2.0f * s21 - s22;

    float edgeSqr = (sobelX * sobelX + sobelY * sobelY);

    bool edge_over_treshold = edgeSqr > (0.07 * 0.07);

    return edge_over_treshold;
}


// specular sobel
bool sobel_specular(vec2 texture_coords)
{
    float s00 = texture(g_albedo_spec, texture_coords + vec2(-offX, -offY)).a;
    float s01 = texture(g_albedo_spec, texture_coords + vec2( 0,   -offY)).a;
    float s02 = texture(g_albedo_spec, texture_coords + vec2( offX, -offY)).a;

    float s10 = texture(g_albedo_spec, texture_coords + vec2(-offX,  0)).a;
    float s12 = texture(g_albedo_spec, texture_coords + vec2( offX,  0)).a;
    
    float s20 = texture(g_albedo_spec, texture_coords + vec2(-offX,  offY)).a;
    float s21 = texture(g_albedo_spec, texture_coords + vec2( 0,    offY)).a;
    float s22 = texture(g_albedo_spec, texture_coords + vec2( offX,  offY)).a;
    
    float sobelX = s00 + 2.0f * s10 + s20 - s02 - 2.0f * s12 - s22;
    float sobelY = s00 + 2.0f * s01 + s02 - s20 - 2.0f * s21 - s22;

    float edgeSqr = (sobelX * sobelX + sobelY * sobelY);
    
    bool edge_over_treshold = edgeSqr > (0.07 * 0.07);


    return false;
}


bool sobel_normal(vec2 texture_coords)
{

    vec3 c11 = texture(g_normal, texture_coords).xyz;
   
    float  s00 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2(-offX, -offY)).xyz) - threadHold);   
    float  s01 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2(   0.0f, -offY)).xyz)  - threadHold);
    float  s02 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2( offX, -offY)).xyz) - threadHold);

    float  s10 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2(-offX,   0.0f)).xyz) - threadHold);
    float  s12 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2( offX,   0.0f)).xyz) - threadHold);
   
    float  s20 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2(-offX, offY)).xyz) - threadHold);
    float  s21 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2(   0.0f,  offY)).xyz) - threadHold);
    float  s22 = max(0.0f, dot(c11, texture(g_normal, texture_coords + vec2( offX, offY)).xyz) - threadHold);
   
    float sobelX = s00 + 2.0f * s10 + s20 - s02 - 2.0f * s12 - s22;
    float sobelY = s00 + 2.0f * s01 + s02 - s20 - 2.0f * s21 - s22;

    float edgeSqr = (sobelX * sobelX + sobelY * sobelY);

    bool edge_over_treshold = edgeSqr > (0.07 * 0.07);

    return edge_over_treshold;
}


void main()
{             
    // retrieve data from G-buffer
    vec3 fragment_position = texture(g_position, texture_coords).rgb;
    vec3 normal = texture(g_normal, texture_coords).rgb;
    vec3 albedo = texture(g_albedo_spec, texture_coords).rgb;
    float specular = texture(g_albedo_spec, texture_coords).a;

    bool salbedo = sobel_albedo(texture_coords);
    bool snormal = sobel_normal(texture_coords); 
    bool sspecular = sobel_specular(texture_coords); 


    vec3 sobel_value = vec3(0.0f);

    // if (salbedo || snormal || sspecular)
    // {
    //     sobel_value = vec3(1.0f);
    // }

    if (snormal)
    {
        sobel_value = vec3(1.0f);
    }


    fragment_color = vec4(sobel_value, 1.0);
}  


