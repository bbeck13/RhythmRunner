#include "Sky.h"
#include <memory>
#include <iostream>

std::shared_ptr<Shape> Sky::shape = std::make_shared<Shape>();
bool Sky::isInitialized = false;

Sky::Sky() : GameObject(Sky::shape) {
  this->position = glm::vec3(0, 0, -10);
  // todo play around with the size
  this->scale = glm::vec3(900, 900, 900);
  this->model = shape;
}

Sky::Sky(glm::vec3 position, glm::vec3 scale) : GameObject(Sky::shape) {
  this->position = position;
  this->scale = scale;
  this->model = shape;
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

ObjectType Sky::GetType() {
  return ObjectType::SCENERY;
}

SecondaryType Sky::GetSecondaryType() {
  return SecondaryType::SKY;
}
