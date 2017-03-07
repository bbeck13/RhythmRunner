#ifndef OBSTACLE_H_
#define OBSTACLE_H_

#include "GameObject.h"

class Obstacle : public GameObject {
 public:
  Obstacle(const std::string& shape_path,
           glm::vec3 position = glm::vec3(),
           glm::vec3 rotation_axis = glm::vec3(),
           float rotation_angle = 0,
           glm::vec3 scale = glm::vec3());
  virtual ~Obstacle();

  ObjectType GetType() override;

 protected:
  // TODO(bnbeck) figure out what should go here
};

#endif
