#include "Collectible.h"

Collectible::Collectible(std::shared_ptr<Shape> shape) : GameObject(shape) {}

Collectible::~Collectible() {}

void Collectible::SetCollected() {
  this->isCollected = true;
}

void Collectible::SetUncollected() {
  this->isCollected = false;
}

bool Collectible::GetCollected() {
  return this->isCollected;
}

ObjectType Collectible::GetType() {
  return ObjectType::COLLECTIBLE;
}
