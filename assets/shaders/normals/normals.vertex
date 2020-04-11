#version 430 core

// Specify the input locations of attributes
layout (location = 0) in vec3 position_in;
layout (location = 1) in vec2 texture_coordinates_in;
layout (location = 2) in vec3 normal_in;

// Specify the Uniforms of the vertex shader
layout (location = 0) uniform mat4 projection_matrix;
layout (location = 1) uniform mat4 model_matrix;
layout (location = 2) uniform mat4 view_matrix;
layout (location = 3) uniform mat3 model_normal_matrix;

// Specify the output of the vertex stage
out vec3 vert_normal;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position_in, 1.0);
    vert_normal = model_normal_matrix * normal_in;
}
