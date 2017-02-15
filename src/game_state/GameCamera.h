// Braden Beck
// Modified: Matthew Stewart - 1/28/17

#ifndef GAME_CAMERA_H_
#define GAME_CAMERA_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>

#include "MatrixStack.h"

#define CAMERA_SPEED 0.4
#define DEGREES_IN_CIRCLE 360.0f
#define VIEWING_ANGLE 160.0f
// Keep the camera this many units in front of the player
#define FORWARD_CAMERA_SPACING 6.0f
// Move the camera X% closer to the target every tick
#define FRACTION_CAMERA_MOVEMENT_PER_TICK 0.2f
// How far the camera is from the player
#define CAMERA_Z_SPACING 9.0f

class GameCamera {
public:
   GameCamera();
   GameCamera(glm::vec3 eyePos, glm::vec3 lookAtPos, glm::vec3 up);

   MatrixStack getView();
   MatrixStack pivot(int width, int height, double xpos, double ypos);

   void setPosition(glm::vec3 new_position);
   glm::vec3 getPosition();

   void setLookAt(glm::vec3 new_lookat);
   glm::vec3 getLookAt();

   void Refresh();

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
