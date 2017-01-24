#ifndef __GROUND_RENDERER_H__
#define __GROUND_RENDERER_H__
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameObject.h"
#include "Graphics.h"
#include "Renderer.h"

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

class GroundRenderer : public Renderer {
public:
   GroundRenderer();
   void init();
   void render();
   void setMaterial(string material);
private:
   int GiboLen;
   GLuint GrndBuffObj;
   GLuint GrndNorBuffObj;
   GLuint GrndTexBuffObj;
   GLuint GIndxBuffObj;
   shared_ptr<Program> program;
   string material;
};

#endif
