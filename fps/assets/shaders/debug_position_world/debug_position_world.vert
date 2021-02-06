#version 450

//-------------------
// Inputs
//-------------------
layout (location = 0) in vec3 position_VS_in; // expected in world already, so expecting a model matrix that does nothing.


//-------------------
// Outputs
//-------------------
layout (location = 0) out vec4 position_FS_in;

//-------------------
// Uniform
//-------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position_VS_in, 1.f);
	position_FS_in = model * vec4(position_VS_in, 1.f);
}
