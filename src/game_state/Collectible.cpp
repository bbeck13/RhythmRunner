#include "Collectible.h"

Collectible::Collectible(const std::string& shape_path,
                         bool isCollected,
                         glm::vec3 position,
                         glm::vec3 rotation_axis,
                         float rotation_angle,
                         glm::vec3 scale)
    : GameObject(shape_path, position, rotation_axis, rotation_angle, scale),
      isCollected(isCollected) {}

Collectible::~Collectible() {}

void Collectible::SetCollected() {
  this->isCollected = true;
}

void Collectible::SetUncollected() {
  this->isCollected = false;
}

bool Collectible::GetCollected() const {
  return this->isCollected;
}

ObjectType Collectible::GetType() {
  return ObjectType::COLLECTIBLE;
}
