#version 450

//-------------------
// Inputs
//-------------------
layout (location = 0) in vec4 position_FS_in;

//-------------------
// Outputs
//-------------------
layout (location = 0) out vec4 fragment_color;

//-------------------
// Uniform
//-------------------
uniform vec4 color;
uniform vec4 highlight_color;

void main()
{
	fragment_color = mix(color, highlight_color, 0.5f);
}

