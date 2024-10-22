#version 430 core

// Specify the input locations of attributes
layout(location = 0) in vec3 position_in;
layout(location = 1) in vec2 texture_coordinates_in;
layout(location = 2) in vec3 normal_in;

// Specify the Uniforms of the vertex shader
layout (location = 0) uniform mat4 projection_matrix;
layout (location = 1) uniform mat4 model_matrix;
layout (location = 2) uniform mat4 view_matrix;
layout (location = 3) uniform mat3 model_normal_matrix;


// unique to gouraud
uniform vec4 light_position;
uniform vec3 light_color;
uniform vec4 material;      // w = ka, x = kd, y = ks, z = n

// Specify the output of the vertex stage
//out vec3 vert_normal;
out vec3 vertex_color;
out vec2 texture_coordinates;

void main()
{
    vec4 world_position_vertex = model_matrix * vec4(position_in, 1.0); // MV
   
    gl_Position = projection_matrix * view_matrix * world_position_vertex;                    // MVP

    vec3 world_position_light = light_position.xyz; // modelMatrix *

    vec3 N = normalize(model_normal_matrix * normal_in);
    vec3 L = normalize(world_position_light.xyz - world_position_vertex.xyz);
    vec3 R = normalize(reflect(L, N));

    vec3 V = normalize(world_position_vertex.xyz - vec3(0));

    vec3 material_color = vec3(0.82, 0.82, 0.82);

    vec3 i_A =  material_color * material.x;
    vec3 i_D = max(0, dot(N, L)) * material_color * material.y;
    vec3 i_S = pow(max(0, dot(R, V)), material.w) * light_color * material.z;

    vertex_color = i_A + i_D + i_S;
    texture_coordinates = texture_coordinates_in;
}