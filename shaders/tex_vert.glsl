#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 MV;

out vec3 vColor;
out vec2 vTexCoord;
out vec3 fragNor;

void main() {

	vec3 lightDir = vec3(1, 1, 1);
  vec4 vPosition;

  /* First model transforms */
  gl_Position = P * MV * vec4(vertPos.xyz, 1.0);

  fragNor = (MV * vec4(vertNor, 0.0)).xyz;
  /* a color that could be blended - or be shading */
  vColor = vec3(max(dot(fragNor, normalize(lightDir)), 0));
  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
}
