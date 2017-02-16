#include "Obstacle.h"

Obstacle::Obstacle(std::shared_ptr<Shape> shape) : GameObject(shape) {}

Obstacle::~Obstacle() {}

ObjectType Obstacle::GetType() {
  return ObjectType::OBSTACLE;
}
