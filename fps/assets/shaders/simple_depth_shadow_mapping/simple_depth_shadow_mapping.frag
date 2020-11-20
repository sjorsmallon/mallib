#version 450 core
out vec4 frag_color;

in VS_OUT {
    vec3 frag_pos;
    vec3 normal;
    vec2 texture_coords;
    vec4 frag_pos_light_space;
} fs_in;

uniform sampler2D diffuse_texture;
uniform sampler2D shadow_map;

uniform vec3 light_pos;
uniform vec3 view_pos;

float ShadowCalculation(vec4 frag_pos_light_space)
{
    // perform perspective divide
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    // transform to [0,1] range
    proj_coords = proj_coords * 0.5f + 0.5f;
    // get closest depth value from light's perspective (using [0,1] range frag_posLight as coords)
    float closest_depth = texture(shadow_map, proj_coords.xy).r; 
    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.normal);
    vec3 light_dir = normalize(light_pos - fs_in.frag_pos);
    float bias = max(0.05f * (1.f - dot(normal, light_dir)), 0.005f);
    // check whether current frag pos is in shadow
    // float shadow = current_depth - bias > closest_depth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.f;
    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r; 
            shadow += current_depth - bias > pcf_depth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.f)
        shadow = 0.f;
        
    return shadow;
}

void main()
{           
    vec3 color = texture(diffuse_texture, fs_in.texture_coords).rgb;
    vec3 normal = normalize(fs_in.normal);
    vec3 lightColor = vec3(0.3f);
    // ambient
    vec3 ambient = 0.3f * color;
    // diffuse
    vec3 light_dir = normalize(light_pos - fs_in.frag_pos);
    float diff = max(dot(light_dir, normal), 0.f);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 view_dir = normalize(view_pos - fs_in.frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = 0.0;
    vec3 halfway_dir = normalize(light_dir + view_dir);  
    spec = pow(max(dot(normal, halfway_dir), 0.f), 64.f);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.frag_pos_light_space);                      
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;    
    
    frag_color = vec4(lighting, 1.0f);
}