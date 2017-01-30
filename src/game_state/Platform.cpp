#include <memory>
#include "Platform.h"
#include <iostream>

std::shared_ptr<Model> Platform::model = std::make_shared<Model>(PLATFORM_MESH);
bool Platform::isInitialized = false;

Platform::Platform(glm::vec3 position) : GameObject(Platform::model) {
  this->position = position;
  // todo play around with the size
  this->scale = glm::vec3(3, .5, .5);
  this->model = model;
}

Platform::Platform(glm::vec3 position, glm::vec3 scale) : GameObject(Platform::model) {
  this->position = position;
  // todo play around with the size
  this->scale = scale;
  this->model = model;
}

Platform::~Platform() {}

std::shared_ptr<Model> Platform::GetPlatformModel() {
	// TODO(jarhar): make sure that the platform model is initialized in the renderer somehow
  return Platform::model;
}
