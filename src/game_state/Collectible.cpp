#include "Collectible.h"

#include "TimingConstants.h"

#define ROTATION_PER_SECOND 6.0
#define ROTATION_PER_TICK (ROTATION_PER_SECOND * SECONDS_PER_TICK)

Collectible::Collectible(const std::string& shape_path,
                         bool isCollected,
                         int ticksSinceCollected,
                         glm::vec3 position,
                         glm::vec3 rotation_axis,
                         float rotation_angle,
                         glm::vec3 scale)
    : GameObject(shape_path, position, rotation_axis, rotation_angle, scale),
      isCollected(isCollected),
      ticksSinceCollected(ticksSinceCollected) {}

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

void Collectible::IncrementTicksCollected(float inc_amt) {
  this->ticksSinceCollected += inc_amt;
}

int Collectible::GetTicksCollected() {
  return std::floor(this->ticksSinceCollected);
}

ObjectType Collectible::GetType() {
  return ObjectType::COLLECTIBLE;
}

void Collectible::Animate(float time_warp) {
  // spin to win
  SetRotationAxis(glm::vec3(0, 1, 0));
  SetRotationAngle(GetRotationAngle() + ROTATION_PER_TICK * time_warp);
}
