#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 MV;
out vec3 fragNor;
out vec3 diff_color;
out vec3 amb_color;
out float f_light_pos;
out float f_exp;
out vec3 f_light_color;

void main() {
  gl_Position = P * V * MV * vertPos;
  fragNor = (MV * vec4(vertNor, 0.0)).xyz;
  amb_color = vec3(0.0, 0.0, 0.0);
  diff_color = vec3(0.1, 0.3, 0.3);
  f_light_pos = 25;
  f_exp = 32;
  f_light_color = vec3(0.4, 0.1, 0.8);
}