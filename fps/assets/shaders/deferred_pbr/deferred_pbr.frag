#version 450 core




in sampler2D texture_albedo;
in sampler2D texture_normal;
in sampler2D texture_metallic;
in sampler2D texture_roughness;
in sampler2D texture_ao;


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

