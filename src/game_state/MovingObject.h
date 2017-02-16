#ifndef MOVING_OBJECT_
#define MOVING_OBJECT_

#include <vector>
#include <glm/glm.hpp>
#include <memory>

#include "GameObject.h"
#include "Shape.h"
#include "AxisAlignedBox.h"

class MovingObject {
 public:
  MovingObject(std::shared_ptr<std::vector<glm::vec3>> path,
               glm::vec3 position,
               float velocity);

  MovingObject(std::shared_ptr<std::vector<glm::vec3>> path,
               glm::vec3 position,
               glm::vec3 velocity);

  glm::vec3 GetVelocity();

  std::shared_ptr<std::vector<glm::vec3>> GetPath();

  glm::vec3 updatePosition(glm::vec3 position);

  glm::vec3 GetMovementVector();

  glm::vec3 GetOriginalPosition();

  AxisAlignedBox GetFullBox(std::shared_ptr<Shape> model, glm::vec3 scale);

  void Reset();

  void SetVelocity(float velocity);

  void SetPath(std::shared_ptr<std::vector<glm::vec3>> path);

 protected:
  std::shared_ptr<std::vector<glm::vec3>> path;
  glm::vec3 movementVector;
  int currentDir;
  glm::vec3 velocity;

  // In case I change it during the level
  glm::vec3 origionalVelocity;
  glm::vec3 originalPosition;
  std::shared_ptr<std::vector<glm::vec3>> origionalPath;

  static double distance(glm::vec3 one, glm::vec3 two);
  static glm::vec3 calculateMovementVector(glm::vec3 goal, glm::vec3 start);
};

#endif
