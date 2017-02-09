#ifndef MOVING_OBJECT_
#define MOVING_OBJECT_

#include <vector>
#include <glm/glm.hpp>
#include <memory>

#include "GameObject.h"
#include "Shape.h"

class MovingObject {
 public:

  virtual float GetVelocity() {
    return velocity;
  }

  virtual std::shared_ptr<std::vector<glm::vec3>> GetPath() {
    return path;
  }

  virtual void SetVelocity(float velocity) {
    this->velocity = velocity;
  }

  void SetPath(std::shared_ptr<std::vector<glm::vec3>> path) {
    this->path = path;
  }

 protected:
  std::shared_ptr<std::vector<glm::vec3>> path;
  float velocity;
};

#endif
