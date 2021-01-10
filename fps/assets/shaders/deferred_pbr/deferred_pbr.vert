#version 430 core

//@Note(Sjors): the Normal that is used here is not the default normal (i.e. normal_matrix * inverse(tranpose(model))),
// but a normal in a different space (model_matrix * normal). I don't know why.

//-------------------
// Inputs
//-------------------
layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec2 texture_coords_VS_in;

//-------------------
// Outputs
//-------------------
layout (location = 0) out vec3 fragment_position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec2 texture_coords;

//-------------------
// uniforms
//-------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 world_position = model * vec4(position_VS_in, 1.0);
    fragment_position = world_position.xyz; 
    texture_coords = texture_coords_VS_in;
    
    //@IC(Sjors): see note above or in post_deferred_pbr.frag
    // mat3 normal_matrix = inverse(transpose(mat3(model)));
    mat3 normal_matrix = mat3(model);
    normal = normalize(normal_matrix * normal_VS_in);

    gl_Position = projection * view * world_position;
}