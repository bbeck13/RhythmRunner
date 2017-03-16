#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D Texture0;

void main() {
  //color = vec4(ParticleColor.xyz, texture(Texture0, TexCoords).w);
  //float alpha = texture(Texture0, gl_PointCoord).r;

  float alpha = texture(Texture0, TexCoords).r;
  if (alpha < 0.001) {
    discard;
  }

  color = vec4(ParticleColor.rgb, alpha);
  //color = vec4(1, 1, 1, alpha);
}
