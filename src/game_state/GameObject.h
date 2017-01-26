// Joseph Arhar

#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <memory>

#include "Shape.h"

// GameObjects are 3D entities which can be rendered
class GameObject {
 public:
  GameObject(std::shared_ptr<Shape> model);
  virtual ~GameObject();

  std::shared_ptr<Shape> GetModel();

  glm::vec3 GetPosition();
  glm::vec3 GetDirection();
  float GetScale();

  void SetPosition(glm::vec3 position);
  void SetDirection(glm::vec3 direction);
  void SetScale(float scale);

 private:
  std::shared_ptr<Shape> model;

  glm::vec3 position;
  glm::vec3 direction;
  float scale;
};

#endif
