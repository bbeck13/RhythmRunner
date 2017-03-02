#include "MoonRock.h"
#include <memory>
#include <iostream>

namespace gameobject {
std::shared_ptr<Shape> MoonRock::shape = std::make_shared<Shape>();
bool MoonRock::isInitialized = false;

MoonRock::MoonRock() : GameObject(MoonRock::shape) {
  this->position = glm::vec3(0, 0, 0);
  // todo play around with the size
  this->scale = glm::vec3(1, 1, 1);
  this->model = shape;
}

MoonRock::MoonRock(glm::vec3 position, glm::vec3 scale)
    : GameObject(MoonRock::shape) {
  this->position = position;
  this->scale = scale;
  this->model = shape;
}
MoonRock::MoonRock(glm::vec3 position,
                   glm::vec3 scale,
                   glm::vec3 rotation_axis,
                   float rotaiton_angle)
    : GameObject(MoonRock::shape) {
  this->position = position;
  this->scale = scale;
  this->rotation_axis = rotation_axis;
  this->rotation_angle = rotaiton_angle;
}

MoonRock::~MoonRock() {}

std::shared_ptr<Shape> MoonRock::GetModel() {
  if (!MoonRock::isInitialized) {
    MoonRock::shape->loadMesh(std::string(ASSET_DIR) + "/" +
                              std::string(ROCK_MESH));
    MoonRock::shape->init();
    MoonRock::isInitialized = true;
  }
  return MoonRock::shape;
}

ObjectType MoonRock::GetType() {
  return ObjectType::OBSTACLE;
}

SecondaryType MoonRock::GetSecondaryType() {
  return SecondaryType::MOONROCK;
}
}
