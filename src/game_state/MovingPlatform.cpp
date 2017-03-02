#include "MovingPlatform.h"

namespace gameobject {
std::shared_ptr<Shape> MovingPlatform::platform = std::make_shared<Shape>();
bool MovingPlatform::isInitialized = false;

MovingPlatform::MovingPlatform()
    : MovingObject(std::vector<glm::vec3>(), glm::vec3(0, 0, 0), 0.0f),
      Obstacle(MovingPlatform::platform) {}

MovingPlatform::MovingPlatform(glm::vec3 position, std::vector<glm::vec3> path)
    : MovingObject(path, position, 0.01f), Obstacle(MovingPlatform::platform) {
  this->position = position;
  this->scale = glm::vec3(3, .6, 2.0f);
  this->model = platform;
}

MovingPlatform::MovingPlatform(glm::vec3 position,
                               glm::vec3 scale,
                               glm::vec3 rotation_axis,
                               float rotation_angle,
                               glm::vec3 velocity,
                               std::vector<glm::vec3> path)
    : MovingObject(path, position, velocity),
      Obstacle(MovingPlatform::platform) {
  this->position = position;
  this->scale = scale;
  this->rotation_angle = rotation_angle;
  this->rotation_axis = rotation_axis;
}

MovingPlatform::MovingPlatform(glm::vec3 position,
                               std::vector<glm::vec3> path,
                               float velocity)
    : MovingObject(path, position, velocity),
      Obstacle(MovingPlatform::platform) {
  this->position = position;
  this->scale = glm::vec3(3, .6, 2.0f);
  this->model = platform;
}

MovingPlatform::~MovingPlatform() {}

std::shared_ptr<Shape> MovingPlatform::GetModel() {
  if (!MovingPlatform::isInitialized) {
    MovingPlatform::platform->loadMesh(std::string(ASSET_DIR) + "/" +
                                       std::string(MOVING_PLATFORM_MESH));
    MovingPlatform::platform->init();
    MovingPlatform::isInitialized = true;
  }
  return MovingPlatform::platform;
}

ObjectType MovingPlatform::GetType() {
  return ObjectType::OBSTACLE;
}

SecondaryType MovingPlatform::GetSecondaryType() {
  return SecondaryType::MOVING_PLATFORM;
}
}
