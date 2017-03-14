#version  330 core
layout(location = 0) in vec4 vertPos;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 P;
uniform vec3 Offset;
uniform vec4 Color;

void main()
{
    float scale = 1.0f;
    TexCoords = vertPos.zw;
    ParticleColor = Color;
    gl_Position = P * vec4((vertPos.xy * scale) + Offset.xy, Offset.z, 1.0);

}
