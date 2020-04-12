#version 430
// Fragment shader

layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_normal;

// uniform float frequency;

out vec4 fColor;

void main() {
  vec3 light_vector = vec3(1.0,0.0,0.0);

  float theta = dot(normalize(vert_normal),light_vector);

  float frequency = 60.0f;

  theta = sin(frequency * theta);
  vec3 alternating_colour;

  if (theta > 0) {
      alternating_colour = vec3(1,0,1);
  }
  else {
      alternating_colour = vec3(0,0,0);
  }


  fColor = vec4(alternating_colour, 1.0);

}