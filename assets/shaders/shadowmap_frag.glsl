#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 ShadowCoord;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform sampler2DShadow shadowMap;

void main(){
   float visibility = 1.0;
   if ( texture( shadowMap, ShadowCoord.xy ).z  <  ShadowCoord.z){
    visibility = 0.5;
}

   color = MaterialAmbientColor + visibility * MaterialDiffuseColor * LightColor * LightPower * cosTheta+ visibility * MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5);

}