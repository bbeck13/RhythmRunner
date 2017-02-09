#ifndef OBSTACLE_H_
#define OBSTACLE_H_
#include "GameObject.h"
#include "Shape.h"

class Obstacle : public GameObject {
 public:
  Obstacle(std::shared_ptr<Shape> shape);
  ~Obstacle();

  ObjectType GetType() override;

 protected:
  // TODO(bnbeck) figure out what should go here
};

#endif
