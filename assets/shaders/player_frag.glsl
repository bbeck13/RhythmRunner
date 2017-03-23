#version 330 core
uniform sampler2D Texture0;

in vec3 fragNor;
in vec4 fragPos;
in vec2 fragTexCoord;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 brightColor;

void main() {
   float amb_str = 0.5f;
   vec3 ambient = amb_str * vec3(1, 1, 1);
   vec3 norm = normalize(fragNor);
   vec3 lightDir = normalize(vec3(150, 50, -4) - fragPos.xyz);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * vec3(1, 1, 1);
   vec3 result = (diffuse + ambient) * vec3(1, 1, 1);
   vec4 surfaceColor = texture(Texture0, fragTexCoord);
   color = vec4(result * surfaceColor.rgb, surfaceColor.a);
   float brightness = dot(color, vec4(0.3126, 0.5152, 0.2722, 0.0));
   if (brightness > 0.3)
      brightColor = color * 1.2;
   else
      brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
