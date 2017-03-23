#version 330 core
out vec4 color;
in vec2 fragTexCoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

void main()
{
   const float gamma = 1.2;
   vec3 hdrColor = texture(scene, fragTexCoord).rgb;
   vec3 bloomColor = texture(bloomBlur, fragTexCoord).rgb;
   if(bloom)
      hdrColor += bloomColor * exposure; // additive blending
   // tone mapping
   vec3 result = vec3(1.0) - exp(-hdrColor);
   result = pow(result, vec3(1.0 / gamma));
   color = vec4(result, 1.0f);
}
