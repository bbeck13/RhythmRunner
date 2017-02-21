#include "PlainRock.h"
#include <memory>
#include <iostream>

namespace gameobject {
std::shared_ptr<Shape> PlainRock::shape = std::make_shared<Shape>();
bool PlainRock::isInitialized = false;

PlainRock::PlainRock() : GameObject(PlainRock::shape) {
  this->position = glm::vec3(0, 0, 0);
  // todo play around with the size
  this->scale = glm::vec3(1, 1, 1);
  this->model = shape;
}

PlainRock::PlainRock(glm::vec3 position, glm::vec3 scale) : GameObject(PlainRock::shape) {
  this->position = position;
  this->scale = scale;
  this->model = shape;
}

PlainRock::~PlainRock() {}

std::shared_ptr<Shape> PlainRock::GetModel() {
  if (!PlainRock::isInitialized) {
    PlainRock::shape->loadMesh(std::string(ASSET_DIR) + "/" +
                          std::string(PLAIN_ROCK_MESH));
    PlainRock::shape->init();
    PlainRock::isInitialized = true;
  }
  return PlainRock::shape;
}

ObjectType PlainRock::GetType() {
  return ObjectType::SCENERY;
}

SecondaryType PlainRock::GetSecondaryType() {
  return SecondaryType::PLAINROCK;
}
}
