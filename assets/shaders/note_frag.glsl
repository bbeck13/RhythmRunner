#version 330 core
in vec3 fragNor;
in vec4 fragPos;
in vec3 obj_color_out;
out vec4 color;

void main() {
  float amb_str = 0.5f;
  vec3 ambient = amb_str * vec3(1, 1, 1);
  vec3 norm = normalize(fragNor);
  vec3 lightDir = normalize(vec3(150, 50, -4) - fragPos.xyz);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * vec3(1, 1, 1);
  vec3 result = (diffuse + ambient) * vec3(obj_color_out);
  color = vec4(result, 1.0);
}
