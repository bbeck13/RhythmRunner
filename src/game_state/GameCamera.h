// Braden Beck
// Modified: Matthew Stewart - 1/28/17

#ifndef GAME_CAMERA_H_
#define GAME_CAMERA_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>

#include "MatrixStack.h"

#define CAMERA_SPEED 0.4
#define DEGREES_IN_CIRCLE 360.0f
#define VIEWING_ANGLE 160.0f

class GameCamera {
public:
   GameCamera();
   GameCamera(glm::vec3 eyePos, glm::vec3 lookAtPos, glm::vec3 up);

   MatrixStack getView();
   MatrixStack pivot(int width, int height, double xpos, double ypos);

private:
   MatrixStack ViewMatrix = MatrixStack();
   glm::vec3 eyePos = glm::vec3(0,1,0);
   glm::vec3 lookAtPos = glm::vec3(0,0,-5);
   glm::vec3 up = glm::vec3(0, 1, 0);
   float obeta = -(M_PI_2);
   float oalpha = 0;
   float beta = 0;
   float alpha = 0;
   float limit = DEGREES_IN_CIRCLE / M_PI / VIEWING_ANGLE;

};

#endif
