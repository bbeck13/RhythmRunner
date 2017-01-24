#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "HitBox.h"
#include "Program.h"
#include "MatrixStack.h"
#include "GLSL.h"
#include "Shape.h"
#include "Texture.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"
#include "HitBox.h"
#include "Renderer.h"

using namespace glm;
using namespace std;

class GameObject {
public:
   GameObject();
   ~GameObject();
private:
   shared_ptr<Renderer> *renderer;
};

#endif /*__GAME_OBJECT_H__*/
