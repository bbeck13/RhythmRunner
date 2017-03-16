#version  330 core
layout(location = 0) in vec4 vertPos;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 P;
uniform mat4 V;
uniform vec3 Offset;
uniform vec4 Color;
uniform vec3 CamRight;
uniform vec3 CamUp;

void main()
{
    float scale = 0.3f;
    TexCoords = vertPos.zw;
    ParticleColor = Color;
    vec3 pos = Offset + CamRight * vertPos.z * scale + CamUp * vertPos.w * scale;
    
    gl_Position = P * V * vec4(pos, 1.0f);
}
