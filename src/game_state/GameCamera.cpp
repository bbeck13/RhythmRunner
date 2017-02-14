// Braden Beck
// Modified: Matthew Stewart - 1/28/17

#include "GameCamera.h"

#include <algorithm>

GameCamera::GameCamera() {
   ViewMatrix.pushMatrix();
   ViewMatrix.lookAt(eyePos, lookAtPos, up);
}

GameCamera::GameCamera(glm::vec3 eyePos, glm::vec3 lookAtPos, glm::vec3 up) {
   this->eyePos = eyePos;
   this->lookAtPos = lookAtPos;
   this->up = up;
}

MatrixStack GameCamera::getView() {
   return ViewMatrix;
}

void GameCamera::Refresh() {
   ViewMatrix = MatrixStack();
   ViewMatrix.pushMatrix();
   ViewMatrix.lookAt(eyePos, lookAtPos, up);
}

void GameCamera::setLookAt(glm::vec3 new_lookat) {
   //printf("look at set to: %f %f %f\n", new_lookat.x, new_lookat.y, new_lookat.z);
   lookAtPos = new_lookat;
   Refresh();
}

glm::vec3 GameCamera::getLookAt() {
   return lookAtPos;
}

void GameCamera::setPosition(glm::vec3 new_position) {
   //printf("position set to: %f %f %f\n", new_position.x, new_position.y, new_position.z);
   eyePos = new_position;
   Refresh();
}

glm::vec3 GameCamera::getPosition() {
   return eyePos;
}

MatrixStack GameCamera::pivot(int width, int height, double xpos, double ypos) {
   double dX = (double) width / 2 - xpos;
   double dY = (double) height / 2 - ypos;

   dX *= CAMERA_SPEED;
   dY *= CAMERA_SPEED;
   beta = obeta + ((float)dX/(float)width)*M_PI;

   alpha = oalpha + ((float)dY/(float)height)*M_PI;

   alpha = alpha < 0 ? std::max(alpha, -limit) :
           std::min(alpha, limit);

   float vx = cos(alpha)*cos(beta);
   float vy = sin(alpha);
   float vz = cos(alpha)*cos(M_PI_2-beta);

   lookAtPos = glm::vec3(vx + eyePos.x, vy + eyePos.y, vz + eyePos.z);
   obeta = beta;
   oalpha = alpha;

   ViewMatrix = MatrixStack();
   ViewMatrix.pushMatrix();
   ViewMatrix.lookAt(eyePos, lookAtPos, up);

   return ViewMatrix;
}
