#ifndef __WORLD_H__
#define __WORLD_H__
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"
#include "GameObject.h"

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"
#define FRAMES_PER_SEC 120.0f
#define SEC_PER_FRAME 1 / FRAMES_PER_SEC

using namespace std;
using namespace glm;

class Window;

class Renderer;

class World {
public:
   World(Window *window);
   ~World();
   void setClearColor(vec4 clearColor);
   void init();
   void update();
private:
   Renderer *renderer;
   Window *window;
   vector<GameObject> gameObjects;
   static int GiboLen;
   static GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
};

#endif /*__WORD_H__*/
