#version 330 core

uniform sampler2D Texture1;

in vec2 vTexCoord;
in vec3 vColor;

out vec4 Outcolor;

void main() {
  vec4 BaseColor = vec4(vColor.x, vColor.y, vColor.z, 1);
  vec4 texColor1 = texture(Texture1, vTexCoord);

	if (texColor1.r < 0.2) {
		Outcolor = vec4(vColor.r, vColor.g, vColor.b, 1);}
	else {
  		Outcolor = vec4(texColor1.r*vColor.r, texColor1.g*vColor.g, texColor1.b*vColor.b, 1);
	}
}

