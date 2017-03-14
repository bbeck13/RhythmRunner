#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D Texture0;

void main()
{
    color = (texture(Texture0, TexCoords) * ParticleColor);
   //color = vec4(.5,.5,0.0,1.0);
}
