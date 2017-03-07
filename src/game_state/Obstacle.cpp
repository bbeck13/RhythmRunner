#include "Obstacle.h"

Obstacle::Obstacle(const std::string& shape_path,
                   glm::vec3 position,
                   glm::vec3 rotation_axis,
                   float rotation_angle,
                   glm::vec3 scale)
    : GameObject(shape_path, position, rotation_axis, rotation_angle, scale) {}

Obstacle::~Obstacle() {}

ObjectType Obstacle::GetType() {
  return ObjectType::OBSTACLE;
}
