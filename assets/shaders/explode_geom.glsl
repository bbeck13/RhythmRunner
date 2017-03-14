#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 geomNor [];
in vec4 geomPos [];
in vec3 obj_color [];

out vec3 fragNor;
out vec4 fragPos;
out vec3 obj_color_out;

uniform int isCollected;
uniform int timeCollected;

vec4 explode(vec4 position, vec3 normal) {
   vec3 direction = normal * timeCollected/3.0;
   return position + vec4(direction, 0.0f); 
}

void main() {
   fragNor = geomNor[0];
   fragPos = geomPos[0];
   obj_color_out = obj_color[0];

   if (isCollected == 1) {   
      gl_Position = explode(gl_in[0].gl_Position, geomNor[0]);
      EmitVertex();

      gl_Position = explode(gl_in[1].gl_Position, geomNor[0]);
      EmitVertex();

      gl_Position = explode(gl_in[2].gl_Position, geomNor[0]);
      EmitVertex();

      EndPrimitive();
   } else {
      gl_Position = gl_in[0].gl_Position;
      EmitVertex();

      gl_Position = gl_in[1].gl_Position;
      EmitVertex();

      gl_Position = gl_in[2].gl_Position;
      EmitVertex();

      EndPrimitive();
   }
}
