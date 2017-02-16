#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 MV;
out vec3 fragNor;
out vec4 fragPos;
out vec2 vTexCoord;

void main() {
  gl_Position = P * V * MV * vertPos;
  fragNor = (MV * vec4(vertNor, 0.0)).xyz;
  fragPos = vec4(MV * vec4(vertPos.xyz, 1.0));
  vTexCoord = vertTex; 
}
