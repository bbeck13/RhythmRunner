#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 MV;
uniform mat4 LS;

uniform vec3 lightDir;

out vec3 fragNor;
out vec4 fragPos;
out vec2 fragTexCoord;
out vec4 fPosLS;
out vec3 vColor;

void main() {
   gl_Position = P * V * MV * vertPos;
   fragNor = (MV * vec4(vertNor, 0.0)).xyz;
   fragPos = vec4(MV * vec4(vertPos.xyz, 1.0));
   fragTexCoord = vertTex;
   fPosLS = LS*fragPos;
   /* a color that could be blended - or be shading */
   vColor = vec3(max(dot(fragNor, normalize(lightDir)), 0));
}
