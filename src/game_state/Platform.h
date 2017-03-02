// bnbeck

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <glm/ext.hpp>

#include "GameObject.h"
#include "Obstacle.h"
#include "json.hpp"

#define PLATFORM_MESH "models/platform.obj"

namespace gameobject {
class Platform : public Obstacle {
 public:
  Platform();
  Platform(glm::vec3 position);
  Platform(glm::vec3 position, glm::vec3 scale);
  Platform(glm::vec3 position,
           glm::vec3 scale,
           glm::vec3 rotation_axis,
           float rotaiton_angle);
  ~Platform();

  std::shared_ptr<Shape> GetModel() override;
  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Shape> platform;
  static bool isInitialized;
};
}
#endif
