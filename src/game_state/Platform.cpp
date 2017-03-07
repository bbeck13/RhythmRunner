#include "Platform.h"

#include <memory>
#include <iostream>

namespace gameobject {

Platform::Platform(glm::vec3 position,
                   glm::vec3 scale,
                   glm::vec3 rotation_axis,
                   float rotation_angle)
    : Obstacle(PLATFORM_MESH, position, rotation_axis, rotation_angle, scale) {}

Platform::~Platform() {}

SecondaryType Platform::GetSecondaryType() {
  return SecondaryType::PLATFORM;
}
}
