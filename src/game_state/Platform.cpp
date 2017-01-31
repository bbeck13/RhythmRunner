#include <memory>
#include "Platform.h"
#include <iostream>

std::shared_ptr<Shape> Platform::platform = std::make_shared<Shape>();
bool Platform::isInitialized = false;

Platform::Platform(glm::vec3 position) : GameObject(Platform::platform) {
  this->position = position;
  // todo play around with the size
  this->scale = glm::vec3(3, .5, .5);
  this->model = platform;
}

Platform::Platform(glm::vec3 position, glm::vec3 scale) : GameObject(Platform::platform) {
  this->position = position;
  // todo play around with the size
  this->scale = scale;
  this->model = platform;
}

Platform::~Platform() {}

std::shared_ptr<Shape> Platform::GetPlatformShape() {
  if (!isInitialized) {
    platform->loadMesh(PLATFORM_MESH);
    platform->init();
    isInitialized = true;
  }
  return platform;
}
