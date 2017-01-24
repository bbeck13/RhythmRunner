#ifndef __CAMERA_H__
#define __CAMERA_H__

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "MatrixStack.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define CAMERA_SPEED 0.4
#define DEGREES_IN_CIRCLE 360.0f
#define VIEWING_ANGLE 160.0f

using namespace glm;
class Camera {
public:
   Camera();
   Camera(vec3 eyePos, vec3 lookAtPos, vec3 up);
   MatrixStack getView();
   MatrixStack pivot(int width, int height, double xpos, double ypos);

private:
   MatrixStack ViewMatrix = MatrixStack();
   vec3 eyePos = vec3(0,0,0);
   vec3 lookAtPos = vec3(0,0,-5);
   vec3 up = vec3(0, 1, 0);
   float obeta = -(M_PI_2);
   float oalpha = 0;
   float beta = 0;
   float alpha = 0;
   float limit = DEGREES_IN_CIRCLE / M_PI / VIEWING_ANGLE;
};

#endif /*__CAMERA_H__*/
