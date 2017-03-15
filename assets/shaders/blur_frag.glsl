#version 330 core
out vec4 color;
in vec2 fragTexCoord;

uniform sampler2D image;
uniform bool horizontal;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
  vec2 tex_offset = 2.0 / textureSize(image, 0); // gets size of single texel
  vec3 result = texture(image, fragTexCoord).rgb * weight[0];
  if(horizontal)
  {
    for(int i = 1; i < 5; ++i)
    {
      result += texture(image, fragTexCoord + vec2(tex_offset.x * i, 0.0)).xyz * weight[i];
      result += texture(image, fragTexCoord - vec2(tex_offset.x * i, 0.0)).xyz * weight[i];
    }
  }
  else
  {
    for(int i = 1; i < 5; ++i)
    {
      result += texture(image, fragTexCoord + vec2(0.0, tex_offset.y * i)).xyz * weight[i];
      result += texture(image, fragTexCoord - vec2(0.0, tex_offset.y * i)).xyz * weight[i];
    }
  }
  color = vec4(result, 1.0);
}
