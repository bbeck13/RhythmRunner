// Braden Beck
// Modified: Matthew Stewart - 1/28/17

#ifndef GAME_CAMERA_H_
#define GAME_CAMERA_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>

#include "MatrixStack.h"
#include "Player.h"

#define CAMERA_SPEED 0.4
#define DEGREES_IN_CIRCLE 360.0f
#define VIEWING_ANGLE 160.0f
// Keep the look at this many units in front of the player
#define FORWARD_CAMERA_SPACING 5.0f
// How fast the camera moves when rotating around look at
#define CAMERA_YAW_MOVE 30.0f
// How far the camera is from the player
#define CAMERA_Z_SPACING 12.0f
#define CAMERA_Y_SPACING 0.2f
#define CAMERA_VIEW_1_X -6
#define CAMERA_VIEW_1_Y .5
#define CAMERA_VIEW_1_Z 3

class GameCamera {
 public:
  GameCamera();
  GameCamera(glm::vec3 eyePos, glm::vec3 lookAtPos, glm::vec3 up);

  MatrixStack getView();
  MatrixStack pivot(int width, int height, double xpos, double ypos);

  void revolveAroundLookAt(float radiansVertical, float radiansHorizontal);

  void setPosition(glm::vec3 new_position);
  glm::vec3 getPosition();
  glm::vec3 getUp();
  glm::vec3 GetCameraPlayerSpacing();

  void setLookAt(glm::vec3 new_lookat);
  glm::vec3 getLookAt();

  void SetCameraPlayerSpacing(glm::vec3 camera_player_spacing);

  void Reset();

  void Refresh();

 private:
  glm::vec3 camera_player_spacing;
  MatrixStack ViewMatrix = MatrixStack();
  glm::vec3 lookAtPos = Player::INITIAL_POSITION +
                        glm::vec3(FORWARD_CAMERA_SPACING, 0, 0);
  glm::vec3 eyePos = lookAtPos + glm::vec3(-8,2,1);
  glm::vec3 up = glm::vec3(0, 1, 0);
  float obeta = -(M_PI_2);
  float oalpha = 0;
  float beta = 0;
  float alpha = 0;
  float limit = DEGREES_IN_CIRCLE / M_PI / VIEWING_ANGLE;
};

#endif
