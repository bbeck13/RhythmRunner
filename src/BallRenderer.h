#ifndef __BALL_RENDERER_H__
#define __BALL_RENDERER_H__
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

class BallRenderer : public Renderer {
public:
   BallRenderer();
   void init();
   void render();
   void setMaterial(string material);
private:
   shared_ptr<Program> program;
   shared_ptr<Shape> shape;
   string material;
};

#endif
