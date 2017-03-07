#ifndef COLLECTIBLE_H_
#define COLLECTIBLE_H_

#include "GameObject.h"
#include "Shape.h"

class Collectible : public GameObject {
 public:
  Collectible(const std::string& shape_path,
              bool isCollected = false,
              glm::vec3 position = glm::vec3(),
              glm::vec3 rotation_axis = glm::vec3(),
              float rotation_angle = 0,
              glm::vec3 scale = glm::vec3());
  virtual ~Collectible();

  void SetCollected();
  void SetUncollected();
  bool GetCollected() const;

  ObjectType GetType() override;

 protected:
  bool isCollected;
};

#endif
