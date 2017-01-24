#version  330 core
layout(location = 0) in vec4 PositionVector;
layout(location = 1) in vec3 NormalVector;

uniform mat4 ProjMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

out vec3 VertexNormal;
out vec4 WorldSpaceVector;

void main() {
   WorldSpaceVector = ViewMatrix * ModelMatrix * PositionVector;
   gl_Position = ProjMatrix * ViewMatrix * ModelMatrix * PositionVector;

   VertexNormal = (ViewMatrix * ModelMatrix * vec4(NormalVector, 0)).xyz;
}
