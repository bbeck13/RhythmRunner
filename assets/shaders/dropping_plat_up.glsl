#version 330 core
uniform sampler2D Texture0;
uniform sampler2D SkyTexture0;

in vec3 fragNor;
in vec4 fragPos;
in vec2 fragTexCoord;

out vec4 color;

#define MAX_LIGHTS 12

uniform int numLights;
uniform struct Light {
   vec4 position;
   vec3 color;
   float attenuation;
   float ambient;
   float angle;
   float direction;
} allLights[MAX_LIGHTS];

vec3 ComputeLight(Light light, vec4 surfaceColor, vec3 normal, vec4 position) {
   vec3 surfaceToLight;
   float attenuation = 1.0;
   surfaceToLight = normalize(light.position.xyz);
   attenuation = 1.0;
   vec3 ambient = light.ambient * surfaceColor.rgb * light.color;
   float diffuseCoef = max(0.0, dot(normal, surfaceToLight));
   vec3 diffuse = diffuseCoef * surfaceColor.rgb * light.color;
   return ambient + attenuation*(diffuse);
}

void main() {
   Light light;
   light.position = vec4(100, 1000, -4, 0);
   light.color = light.color + 0.5;
   light.color = texture(SkyTexture0, fragTexCoord).rgb + 0.5;
   light.attenuation = 10;
   light.ambient = 0.8;
   light.angle = 0;
   light.direction = 0;
   vec4 surfaceColor = texture(Texture0, fragTexCoord) + vec4(0.3, 0.9, 0.3, 1);
   color = vec4(ComputeLight(light, surfaceColor, fragNor, fragPos), 1);
}
