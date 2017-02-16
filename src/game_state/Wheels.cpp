#include "Wheels.h"
#include <memory>
#include <iostream>

std::shared_ptr<Shape> Wheels::shape = std::make_shared<Shape>();
bool Wheels::isInitialized = false;

Wheels::Wheels(glm::vec3 position) : Collectible(Wheels::shape) {
  this->position = position;
  // todo play around with the size
  this->scale = glm::vec3(0.5, 0.5, 0.5);
  this->model = shape;
  this->isCollected = false;
}

Wheels::Wheels(glm::vec3 position, glm::vec3 scale) : Collectible(Wheels::shape) {
  this->position = position;
  this->scale = scale;
  this->model = shape;
  this->isCollected = false;
}

Wheels::~Wheels() {}

std::shared_ptr<Shape> Wheels::GetModel() {
  if (!Wheels::isInitialized) {
    Wheels::shape->loadMesh(std::string(ASSET_DIR) + "/" +
                          std::string(NOTE_MESH));
    Wheels::shape->init();
    Wheels::isInitialized = true;
  }
  return Wheels::shape;
}

SecondaryType Wheels::GetSecondaryType() {
  return SecondaryType::NOTE;
}
