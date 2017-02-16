
#ifndef MOVING_PLATFORM_H_
#define MOVING_PLATFORM_H_

#include <glm/ext.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"
#include "Obstacle.h"
#include "Shape.h"
#include "MovingObject.h"

#define MOVING_PLATFORM_MESH "models/platform.obj"

class MovingPlatform : public MovingObject, public Obstacle {
 public:
  MovingPlatform(glm::vec3 position,
                 std::shared_ptr<std::vector<glm::vec3>> path);
  MovingPlatform(glm::vec3 position,
                 std::shared_ptr<std::vector<glm::vec3>> path,
                 float velocity);
  ~MovingPlatform();

  std::shared_ptr<Shape> GetModel() override;
  ObjectType GetType() override;
  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Shape> platform;
  static bool isInitialized;
};

#endif
