#include "DroppingPlatform.h"

std::shared_ptr<Shape> DroppingPlatform::shape = std::make_shared<Shape>();
bool DroppingPlatform::isInitialized = false;

DroppingPlatform::DroppingPlatform(glm::vec3 position)
    : Obstacle(DroppingPlatform::shape), originalPosition(position) {
  this->position = position - glm::vec3(0, 0, 0.1);
  this->scale = glm::vec3(3, .5, 3);
  this->model = shape;
  dropVel = -0.1f;
  dropping = false;
}

DroppingPlatform::DroppingPlatform(glm::vec3 position, glm::vec3 scale)
    : Obstacle(DroppingPlatform::shape), originalPosition(position) {
  this->position = position - glm::vec3(0, 0, 0.1);
  this->scale = glm::vec3(3, .5, 3);
  this->model = shape;
  dropVel = -0.1f;
  dropping = false;
}

void DroppingPlatform::SetDropping() {
  dropping = true;
}

float DroppingPlatform::GetYVelocity() {
  return dropVel;
}

void DroppingPlatform::Reset() {
  position = originalPosition;
  dropping = false;
}

bool DroppingPlatform::IsDropping() {
  return dropping;
}

std::shared_ptr<Shape> DroppingPlatform::GetModel() {
  if (!DroppingPlatform::isInitialized) {
    DroppingPlatform::shape->loadMesh(ASSET_DIR "/" PLATFORM_MESH);
    DroppingPlatform::shape->init();
    DroppingPlatform::isInitialized = true;
  }
  return DroppingPlatform::shape;
}
SecondaryType DroppingPlatform::GetSecondaryType() {
  return SecondaryType::DROPPING_PLATFORM;
}
