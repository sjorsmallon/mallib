#version 430 core

//--- Inputs ---------------------------------------
layout (location = 0) in vec3 position_in;
layout (location = 1) in vec2 texture_coordinates_in;
layout (location = 2) in vec3 normal_in;


//--- Outputs --------------------------------------
layout (location = 0) out vec3 position_fs;
layout (location = 1) out vec2 texture_coordinates_fs;
layout (location = 2) out vec3 normal_fs;
layout (location = 3) out vec3 world_position_fs;

//--- Uniforms -------------------------------------
layout (location = 0) uniform mat4 projection_matrix;
layout (location = 1) uniform mat4 model_matrix;
layout (location = 2) uniform mat4 view_matrix;
layout (location = 3) uniform mat3 model_normal_matrix;

void main()
{

    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position_in, 1.0);
    position_fs = gl_Position.xyz;

    world_position_fs = vec3(model_matrix * vec4(position_in, 1.0));
    normal_fs = model_normal_matrix * normal_in;
    texture_coordinates_fs = texture_coordinates_in;
}
