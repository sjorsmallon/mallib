#version 430
// Fragment shader

layout (location = 0) in vec4 vertex_position_fs;
layout (location = 1) in vec3 vertex_normal_fs;

// uniform float frequency;

out vec4 fragment_color;

void main() {
  vec3 light_vector = vec3(1.0,0.0,0.0);

  float theta = dot(normalize(vertex_normal_fs),light_vector);

  float frequency = 60.0f;

  theta = sin(frequency * theta);
  vec3 alternating_color;

  if (theta > 0) {
      alternating_color = vec3(1,0,1);
  }
  else {
      alternating_color = vec3(0,0,0);
  }


  fragment_color = vec4(alternating_color, 1.0);

}