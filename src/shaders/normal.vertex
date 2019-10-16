#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec2 texCoordinates_in;
layout (location = 2) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelMatrix;
uniform mat4 modelProjection;
uniform mat4 viewMatrix;
uniform mat3 normalTransform;

// Specify the output of the vertex stage
out vec3 vertNormal;

void main()
{
    gl_Position = modelProjection * viewMatrix * modelMatrix * vec4(vertCoordinates_in, 1.0);
    vertNormal  = normalTransform * vertNormal_in;
}
