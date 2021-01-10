#version 430 core

//@Note(Sjors): the Normal that is used here is not the default normal (i.e. normal_matrix * inverse(tranpose(model))),
// but a normal in a different space (model_matrix * normal). I don't know why.

//-------------------
// Inputs
//-------------------
layout (location = 0) in vec3 position_VS_in;
layout (location = 1) in vec3 normal_VS_in;
layout (location = 2) in vec3 tangent_VS_in;
layout (location = 3) in vec3 bitangent_VS_in;
layout (location = 4) in vec2 texture_coords_VS_in;

//-------------------
// Outputs
//-------------------
layout (location = 0) out vec3 fragment_position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 out_view_position;
layout (location = 3) out vec3 out_light_position;
layout (location = 4) out vec2 texture_coords;


//-------------------
// uniforms
//-------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 view_position;
uniform vec4 light_position;

void main()
{
    vec4 world_position = model * vec4(position_VS_in, 1.0);
    fragment_position = world_position.xyz; 
    texture_coords = texture_coords_VS_in;
    
    //@IC(Sjors): see note above or in post_deferred_pbr.frag
    // mat3 normal_matrix = inverse(transpose(mat3(model)));
    mat3 normal_matrix = mat3(model);
    normal =         normalize(normal_matrix * normal_VS_in);
    normal  = vec3(0.0f, 0.0f, 1.0f); 
    vec3 tangent =   normalize(normal_matrix * tangent_VS_in);
    vec3 bitangent = normalize(normal_matrix * bitangent_VS_in);
    mat3 TBN = transpose(mat3(tangent, bitangent, normal));


    fragment_position = TBN * fragment_position;
    out_view_position = TBN * view_position.xyz;
    out_light_position = TBN * glm::vec3(light_position.xyz);


    gl_Position = projection * view * world_position;
}