#version 330 core
uniform sampler2D Texture0;
uniform sampler2D shadowDepth;

in vec3 fragNor;
in vec4 fragPos;
in vec2 fragTexCoord;
in vec4 fPosLS;
in vec3 vColor;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 brightColor;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {
   float bias = 0.01;
   //1: shift the coordinates from -1, 1 to 0 ,1
   vec3 shift = (LSfPos.xyz + vec3(1.0)) / 2.0;
   //2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy
   float curD = shift.z;
   float lightD = texture(shadowDepth, shift.xy).r;
   //3: compare to the current depth (.z) of the projected depth
   //4: return 1 if the point is shadowed
   if (curD - bias > lightD) {
      return 1;
   }

   return 0.0;
}

void main() {
   float Shade;
   float amb_str = 0.5f;
   vec3 ambient = amb_str * vec3(1, 1, 1);
   vec3 norm = normalize(fragNor);
   vec3 lightDir = normalize(vec3(150, 50, -4) - fragPos.xyz);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * vec3(1, 1, 1);
   vec3 result = (diffuse + ambient) * vec3(1, 1, 1);
   vec4 surfaceColor = texture(Texture0, fragTexCoord);
   color = vec4(result * surfaceColor.rgb, surfaceColor.a);

   vec4 BaseColor = vec4(vColor, 1);
   float brightness = dot(color, vec4(0.3126, 0.5152, 0.2722, 0.0));

   Shade = TestShadow(fPosLS);

   if (brightness > 0.6)
      brightColor = color * 0.75;
   else
      brightColor = vec4(0.0, 0.0, 0.0, 1.0);

   brightColor = amb_str*(surfaceColor) + (1.0-Shade)*surfaceColor*BaseColor;
}
