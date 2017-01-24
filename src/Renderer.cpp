#include "Renderer.h"
#include "Engine.h"

//materials
unordered_map<string, MaterialProprieties_t> Renderer::materials;

MatrixStack Renderer::Perspective;
MatrixStack Renderer::Model;
vector<Renderer*> Renderer::renderers;
Window *window;

Renderer::Renderer() {
   materials[BLUE_PLASTIC] = (MaterialProprieties_t) {
      vec3(0.02, 0.04, 0.2),
           vec3(0.0, 0.16, 0.9),
           vec3(0.14, 0.2, 0.8),
           120.0f
   };
   Perspective = MatrixStack();
   Model = MatrixStack();
}

void Renderer::setMaterial(shared_ptr<Program> program, string key) {
   if (materials.find(key) != materials.end()) {
      MaterialProprieties_t material = materials[key];
      glUniform3f(program->getUniform(AMBIENT), material.ambient.x,
                  material.ambient.y,
                  material.ambient.z);
      glUniform3f(program->getUniform(DIFFUSE), material.diffuse.x,
                  material.diffuse.y,
                  material.diffuse.z);
      glUniform3f(program->getUniform(SPECULAR), material.specular.x,
                  material.specular.y,
                  material.specular.z);
      glUniform1f(program->getUniform(SHINE), material.shine);
   } else {
      glUniform3f(program->getUniform(AMBIENT), 0.0913, 0.00735, 0.0725);
      glUniform3f(program->getUniform(DIFFUSE), 0.38, 0.48, 0.08);
      glUniform3f(program->getUniform(SPECULAR), .5, .9, 0.001);
      glUniform1f(program->getUniform(SHINE), .1);
   }
}

shared_ptr<Program> Renderer::newProgram(string vertexShader, string fragmentShader) {
   shared_ptr<Program> program = make_shared<Program>();
#ifdef DEBUG
   program->setVerbose(true);
#endif
   program->setShaderNames(vertexShader, fragmentShader);
   return program;
}

shared_ptr<Program> Renderer::newDefaultProgram() {
   shared_ptr<Program> program = newProgram(Engine::getResourceDir() + VERTEX_SHADER,
         Engine::getResourceDir() + FRAGMENT_SHADER);
   program->init();
   program->addUniform("ProjMatrix");
   program->addUniform("ViewMatrix");
   program->addUniform("ModelMatrix");
   program->addUniform("AmbientColor");
   program->addUniform("DiffuseColor");
   program->addUniform("SpecularColor");
   program->addUniform("ShineComponent");
   program->addAttribute("PositionVector");
   program->addAttribute("NormalVector");
   program->addAttribute("VertexVector");

   return program;
}

shared_ptr<Shape> Renderer::loadMesh(string mesh) {
   shared_ptr<Shape> shape = make_shared<Shape>();
   shape->loadMesh(mesh);
   shape->resize();
   shape->init();
   return shape;
}
