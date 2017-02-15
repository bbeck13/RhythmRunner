#ifndef DROPPING_PLATFORM_H_
#define DROPPING_PLATFORM_H_

#include <glm/ext.hpp>

#include "Obstacle.h"

#define PLATFORM_MESH "models/platform.obj"

class DroppingPlatform : public Obstacle {
 public:
  DroppingPlatform(glm::vec3 position);
  DroppingPlatform(glm::vec3 position, glm::vec3 scale);

  std::shared_ptr<Shape> GetModel() override;
  SecondaryType GetSecondaryType() override;
  void Reset();
  bool IsDropping();
  void SetDropping();
  float GetYVelocity();

 private:
  float dropVel;
  bool dropping;

  glm::vec3 originalPosition;

  static std::shared_ptr<Shape> shape;
  static bool isInitialized;
};

#endif
