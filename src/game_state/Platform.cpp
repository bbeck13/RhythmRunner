#include <memory>
#include "Platform.h"
#include <iostream>

std::shared_ptr<Shape> Platform::platform = std::make_shared<Shape>();
bool Platform::isInitialized = false;

Platform::Platform(glm::vec3 position) : Obstacle(Platform::platform) {
  this->position = position;
  // todo play around with the size
  this->scale = glm::vec3(4, .5, .5);
  this->model = platform;
}

Platform::Platform(glm::vec3 position, glm::vec3 scale)
    : Obstacle(Platform::platform) {
  this->position = position;
  this->scale = scale;
  this->model = platform;
}

Platform::~Platform() {}

std::shared_ptr<Shape> Platform::GetModel() {
  if (!Platform::isInitialized) {
    Platform::platform->loadMesh(std::string(ASSET_DIR) + "/" +
                                 std::string(PLATFORM_MESH));
    Platform::platform->init();
    Platform::isInitialized = true;
  }
  return Platform::platform;
}

SecondaryType Platform::GetSecondaryType() {
  return SecondaryType::PLATFORM;
}
