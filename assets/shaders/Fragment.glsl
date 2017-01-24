#version  330 core
in vec4 WorldSpaceVector;
in vec3 VertexNormal;
out vec4 Color;

uniform vec3 LightPosition;

uniform vec3 AmbientColor;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform float ShineComponent;

void main() {
   vec3 lightVector = normalize(LightPosition - WorldSpaceVector.xyz);

   float Id = dot(normalize(VertexNormal), lightVector);
   float Is = pow(dot(normalize(VertexNormal),
            normalize(lightVector + WorldSpaceVector.xyz)), ShineComponent);

   Color = vec4(Is * SpecularColor + Id * DiffuseColor + AmbientColor, 1);
}
