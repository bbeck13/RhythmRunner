#include "Sky.h"
#include <memory>
#include <iostream>

std::shared_ptr<Shape> Sky::shape = std::make_shared<Shape>();
bool Sky::isInitialized = false;

Sky::Sky(glm::vec3 position) : Collectible(Sky::shape) {
  this->position = position;
  // todo play around with the size
  this->scale = glm::vec3(20, 20, 20);
  this->model = shape;
  this->isCollected = false;
}

Sky::Sky(glm::vec3 position, glm::vec3 scale) : Collectible(Sky::shape) {
  this->position = position;
  this->scale = scale;
  this->model = shape;
  this->isCollected = false;
}

Sky::~Sky() {}

std::shared_ptr<Shape> Sky::GetModel() {
  if (!Sky::isInitialized) {
    Sky::shape->loadMesh(std::string(ASSET_DIR) + "/" +
                          std::string(SKY_MESH));
    Sky::shape->init();
    Sky::isInitialized = true;
  }
  return Sky::shape;
}

SecondaryType Sky::GetSecondaryType() {
  return SecondaryType::NOTE;
}
