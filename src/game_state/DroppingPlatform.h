#ifndef DROPPING_PLATFORM_H_
#define DROPPING_PLATFORM_H_

#include <glm/ext.hpp>

#include "Obstacle.h"

#define PLATFORM_MESH "models/platform.obj"

namespace gameobject {
class DroppingPlatform : public Obstacle {
 public:
  DroppingPlatform();
  DroppingPlatform(glm::vec3 position);
  DroppingPlatform(glm::vec3 position, glm::vec3 scale);
  DroppingPlatform(glm::vec3 position,
                   glm::vec3 scale,
                   glm::vec3 rotation_axis,
                   float rotaiton_angle,
                   float dropVel,
                   bool dropping);

  std::shared_ptr<Shape> GetModel() override;
  SecondaryType GetSecondaryType() override;
  void Reset();
  bool IsDropping() const;
  void SetDropping();
  float GetYVelocity() const;

 private:
  float dropVel;
  bool dropping;

  glm::vec3 originalPosition;

  static std::shared_ptr<Shape> shape;
  static bool isInitialized;
};
}

#endif
