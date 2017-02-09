#include "MovingPlatform.h"

std::shared_ptr<Shape> MovingPlatform::platform = std::make_shared<Shape>();
bool MovingPlatform::isInitialized = false;

MovingPlatform::MovingPlatform(glm::vec3 position,
                               std::shared_ptr<std::vector<glm::vec3>> path)
    : Obstacle(MovingPlatform::platform) {
  this->position = position;
  this->path = path;
}

MovingPlatform::~MovingPlatform() {}

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
