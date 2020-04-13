#version 430 core

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vert_normal;

// Specify the Uniforms of the fragment shaders
// uniform vec3 light_position; // for example

// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{
	vec3 normal = (0.5f * normalize(vert_normal)) + 0.5f;
    fColor = vec4(normal, 1.0);
}
