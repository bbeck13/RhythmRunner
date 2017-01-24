#include "GroundRenderer.h"

GroundRenderer::GroundRenderer() {
   this->program = newDefaultProgram();
   Renderer::renderers.push_back(this);
}

void GroundRenderer::setMaterial(string material) {
   program->addUniform(AMBIENT);
   program->addUniform(DIFFUSE);
   program->addUniform(SPECULAR);
   program->addUniform(SHINE);
   this->material = material;
}

void GroundRenderer::init() {
   float g_groundSize = 20;
   float g_groundY = -1.5;

   // A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
   float GrndPos[] = {
      -g_groundSize, g_groundY, -g_groundSize,
      -g_groundSize, g_groundY,  g_groundSize,
      g_groundSize, g_groundY,  g_groundSize,
      g_groundSize, g_groundY, -g_groundSize
   };

   float GrndNorm[] = {
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0
   };

   static GLfloat GrndTex[] = {
      0, 0, // back
      0, 1,
      1, 1,
      1, 0
   };

   unsigned short idx[] = {0, 1, 2, 0, 2, 3};

   GLuint VertexArrayID;
   //generate the VAO
   glGenVertexArrays(1, &VertexArrayID);
   glBindVertexArray(VertexArrayID);

   GiboLen = 6;
   glGenBuffers(1, &GrndBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

   glGenBuffers(1, &GrndNorBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

   glGenBuffers(1, &GrndTexBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

   glGenBuffers(1, &GIndxBuffObj);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
}

void GroundRenderer::render() {
   program->bind();

   glUniformMatrix4fv(program->getUniform("ProjMatrix"), 1, GL_FALSE,
                      value_ptr(Perspective.topMatrix()));
   glUniformMatrix4fv(program->getUniform("ModelMatrix"), 1, GL_FALSE,
                      value_ptr(Model.topMatrix()));
   glUniformMatrix4fv(program->getUniform("ViewMatrix"), 1, GL_FALSE,
                      value_ptr(Window::getView().topMatrix()));
   if (!material.empty()) {
      Renderer::setMaterial(this->program, material);
   }
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   // draw!
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
   glDrawElements(GL_TRIANGLES, GiboLen, GL_UNSIGNED_SHORT, 0);
   program->unbind();

}
