#version 330 core
uniform sampler2D Texture0;

in vec3 fragNor;
in vec4 fragPos;
in vec2 fragTexCoord;

out vec4 color;

void main() {
  float amb_str = 0.8f;
  vec3 ambient = amb_str * vec3(1, 1, 1);
  vec3 norm = normalize(fragNor);
  vec3 lightDir = normalize(vec3(150, 50, -4) - fragPos.xyz);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * vec3(1, 1, 1);
  vec3 result = (diffuse + ambient) * vec3(.3, .3, .3);
  vec4 surfaceColor = texture(Texture0, fragTexCoord);
  color = vec4(result * surfaceColor.rgb, surfaceColor.a);
}
