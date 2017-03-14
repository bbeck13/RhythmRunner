#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 MV;
uniform vec3 in_obj_color;
uniform int isCollected;
uniform int timeCollected;
out vec3 obj_color;
out vec3 geomNor;
out vec4 geomPos;

void main() {
  gl_Position = P * V * MV * vertPos;
  geomNor = (MV * vec4(vertNor, 0.0)).xyz;
  geomPos = vec4(MV * vec4(vertPos.xyz, 1.0));
  obj_color = in_obj_color;
}
