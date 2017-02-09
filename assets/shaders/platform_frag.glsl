#version 330 core
uniform sampler2D Texture0;
in vec3 fragNor;
in float f_light_pos;
in float f_exp;
in vec3 f_light_color;
in vec3 amb_color;
in vec3 diff_color;
//out vec4 color;
vec3 new_amb_color;
in vec2 vTexCoord;
in vec3 vColor;

out vec4 Outcolor;

void main() {

  vec4 BaseColor = vec4(vColor.x, vColor.y, vColor.z, 1);
  vec4 texColor0 = texture(Texture0, vTexCoord);
  Outcolor = vec4(texColor0.r*vColor.r, texColor0.g*vColor.g, texColor0.b*vColor.b, 1);


  vec3 lightPos = vec3(f_light_pos, 2, 25);
  vec3 normal = normalize(fragNor);

  float diffuse = max(0, dot(normal, lightPos));
  vec3 toEye = -normalize(vec3(fragNor));
  vec3 h = normalize(toEye + lightPos);
  
  float phong_exp = f_exp;
  float specular = pow(max(0, dot(h, normal)), phong_exp);

  vec3 i = diff_color * (amb_color + f_light_color * diffuse) + f_light_color * specular;

  //color = vec4(i, 1.0);

}
