#version 430
//@note: the location specifier for uniforms and attributes is different. Therefore, we can "reuse" indices.

// Specify the input locations of attributes
layout(location = 0) in vec3 position_in;
layout(location = 1) in vec2 texture_coordinates_in;
layout(location = 2) in vec3 normal_in;

// Specify the Uniforms of the vertex shader
layout (location = 0) uniform mat4 projection_matrix;
layout (location = 1) uniform mat4 model_matrix;
layout (location = 2) uniform mat4 view_matrix;
layout (location = 3) uniform mat3 model_normal_matrix;


// Specify the output of the vertex stage
layout(location = 0) out vec4 vertex_position_fs;
layout(location = 1) out vec3 vertex_normal_fs;

void main()
{
    vertex_position_fs = projection_matrix * view_matrix * model_matrix * vec4(position_in, 1.0);
    gl_Position = vertex_position_fs;

    vertex_normal_fs = model_normal_matrix * normal_in;
}
