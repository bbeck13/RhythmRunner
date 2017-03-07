#include "Sky.h"

#include <memory>
#include <iostream>

Sky::Sky(glm::vec3 position, glm::vec3 scale)
    : GameObject(SKY_MESH, position, glm::vec3(1, 0, 0), 0, scale) {}

Sky::~Sky() {}

ObjectType Sky::GetType() {
  return ObjectType::SCENERY;
}

SecondaryType Sky::GetSecondaryType() {
  return SecondaryType::SKY;
}
