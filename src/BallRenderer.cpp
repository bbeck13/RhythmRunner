#include "BallRenderer.h"

BallRenderer::BallRenderer() {
   this->program = newDefaultProgram();
   this->shape = loadMesh("../assets/models/sphere.obj");
   Renderer::renderers.push_back(this);
}

void BallRenderer::setMaterial(string material) {
   program->addUniform(AMBIENT);
   program->addUniform(DIFFUSE);
   program->addUniform(SPECULAR);
   program->addUniform(SHINE);
   this->material = material;
}

void BallRenderer::init() {
}

void BallRenderer::render() {
   program->bind();
   Model.translate(vec3(0, 0, -5));

   glUniformMatrix4fv(program->getUniform("ProjMatrix"), 1, GL_FALSE,
                      value_ptr(Perspective.topMatrix()));
   glUniformMatrix4fv(program->getUniform("ModelMatrix"), 1, GL_FALSE,
                      value_ptr(Model.topMatrix()));
   glUniformMatrix4fv(program->getUniform("ViewMatrix"), 1, GL_FALSE,
                      value_ptr(Window::getView().topMatrix()));
   if (!material.empty()) {
      Renderer::setMaterial(this->program, material);
   }
   shape->draw(program);
}
