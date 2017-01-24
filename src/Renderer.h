#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <string>
#include <vector>
#include <unordered_map>

#include "Camera.h"
#include "Window.h"
#include "Program.h"
#include "Shape.h"

#define BLUE_PLASTIC "BP"
#define SHADER_DIR "shaders/"
#define VERTEX_SHADER "shaders/Vertex.glsl"
#define FRAGMENT_SHADER "shaders/Fragment.glsl"
#define MODEL_DIR "models/"
#define SPHERE "models/sphere.obj"

using namespace std;
using namespace glm;

typedef struct MaterialProprieties {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   float shine;
} MaterialProprieties_t, *MaterialProprieties_p;

class Renderer {
public:
   Renderer();

   virtual void init() {
      for (Renderer *renderer : renderers) {
         renderer->init();
      }
   }
   virtual void render() {
      Perspective.pushMatrix();
      Perspective.perspective(45.0f, Window::getAspect(), 0.01f, 100.0f);
      for (Renderer *renderer : renderers) {
         Model.pushMatrix();
         Model.loadIdentity();
         renderer->render();
         Model.popMatrix();
      }
      Perspective.popMatrix();
   }

   static shared_ptr<Program> newProgram(string vertexShader, string fragmentShader);
   static shared_ptr<Program> newDefaultProgram();
   static shared_ptr<Shape> loadMesh(string mesh);

private:
   static unordered_map<string, MaterialProprieties_t> materials;
protected:
   static void setMaterial(shared_ptr<Program> program, string material);
   static vector<Renderer*> renderers;
   static MatrixStack Perspective;
   static MatrixStack Model;
};

#endif /*__RENDERER_H__*/
