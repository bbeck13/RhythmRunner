#include "MovingPlatform.h"

std::shared_ptr<Shape> MovingPlatform::platform = std::make_shared<Shape>();
bool MovingPlatform::isInitialized = false;

MovingPlatform::MovingPlatform(glm::vec3 position,
                               std::shared_ptr<std::vector<glm::vec3>> path)
    : MovingObject(path, position, 0.01f), Obstacle(MovingPlatform::platform) {
  this->position = position;
  this->scale = glm::vec3(3, .5, .5);
  this->model = platform;
}
MovingPlatform::MovingPlatform(glm::vec3 position,
                               std::shared_ptr<std::vector<glm::vec3>> path,
                               float velocity)
    : MovingObject(path, position, velocity),
      Obstacle(MovingPlatform::platform) {
  this->position = position;
  this->scale = glm::vec3(3, .5, .5);
  this->model = platform;
}

MovingPlatform::~MovingPlatform() {
  this->position = position;
  this->scale = glm::vec3(3, .5, .5);
  this->model = platform;
}

std::shared_ptr<Shape> MovingPlatform::GetModel() {
  if (!MovingPlatform::isInitialized) {
    MovingPlatform::platform->loadMesh(std::string(ASSET_DIR) + "/" +
                                       std::string(PLATFORM_MESH));
    MovingPlatform::platform->init();
    MovingPlatform::isInitialized = true;
  }
  return MovingPlatform::platform;
}

ObjectType MovingPlatform::GetType() {
  return ObjectType::OBSTACLE;
}
