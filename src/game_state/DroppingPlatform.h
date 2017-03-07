#ifndef DROPPING_PLATFORM_H_
#define DROPPING_PLATFORM_H_

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Obstacle.h"
#include "GameObject.h"

#define PLATFORM_SHAPE "models/platform.obj"

namespace gameobject {
class DroppingPlatform : public Obstacle {
 public:
  DroppingPlatform(glm::vec3 position = glm::vec3(),
                   glm::vec3 scale = glm::vec3(3, .5, 4),
                   glm::vec3 rotation_axis = glm::vec3(1, 0, 0),
                   float rotation_angle = 0,
                   float dropVel = -0.1f,
                   bool dropping = false);

  SecondaryType GetSecondaryType() override;
  void Reset();
  bool IsDropping() const;
  void SetDropping();
  float GetYVelocity() const;

 private:
  float dropVel;
  bool dropping;
  glm::vec3 originalPosition;
};
}

#endif
