#include "Collectible.h"

Collectible::Collectible(const std::string& shape_path,
                         bool isCollected,
                         int ticksSinceCollected,
                         glm::vec3 position,
                         glm::vec3 rotation_axis,
                         float rotation_angle,
                         glm::vec3 scale)
    : GameObject(shape_path, position, rotation_axis, rotation_angle, scale),
      isCollected(isCollected), ticksSinceCollected(ticksSinceCollected) {}

Collectible::~Collectible() {}

void Collectible::SetCollected() {
  this->isCollected = true;
}

void Collectible::SetUncollected() {
  this->isCollected = false;
  this->ticksSinceCollected = 0;
}

bool Collectible::GetCollected() const {
  return this->isCollected;
}

void Collectible::IncrementTicksCollected() {
  this->ticksSinceCollected++;
}

int Collectible::GetTicksCollected() {
  return this->ticksSinceCollected;
}

ObjectType Collectible::GetType() {
  return ObjectType::COLLECTIBLE;
}
