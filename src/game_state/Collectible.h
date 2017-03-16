#ifndef COLLECTIBLE_H_
#define COLLECTIBLE_H_

#include "GameObject.h"
#include "Shape.h"

class Collectible : public GameObject {
 public:
  Collectible(const std::string& shape_path,
              bool isCollected = false,
              int ticksSinceCollected = 0,
              glm::vec3 position = glm::vec3(),
              glm::vec3 rotation_axis = glm::vec3(1, 0, 0),
              float rotation_angle = 0,
              glm::vec3 scale = glm::vec3(1, 1, 1));
  virtual ~Collectible();

  void SetCollected();
  void SetUncollected();
  void IncrementTicksCollected(float inc_amt);
  bool GetCollected() const;
  int GetTicksCollected();

  ObjectType GetType() override;
  void Animate(float time_warp);

 protected:
  bool isCollected;
  float ticksSinceCollected;
};

#endif
