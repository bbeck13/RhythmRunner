// Joseph Arhar

#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <memory>
#include <glm/ext.hpp>

#include <glm/glm.hpp>

#include "game_state/Model.h"
#include "Texture.h"

// GameObjects are 3D entities which can be rendered
class GameObject {
 public:
  GameObject(std::shared_ptr<Model> model);
  virtual ~GameObject();

  std::shared_ptr<Model> GetModel();

  glm::vec3 GetPosition();
  glm::vec3 GetDirection();
  glm::vec3 GetScale();
  std::shared_ptr<Texture> GetTexture();  // Texture may be null

  void SetPosition(glm::vec3 position);
  void SetDirection(glm::vec3 direction);
  void SetScale(glm::vec3 scale);
  void SetTexture(std::shared_ptr<Texture> texture);

 protected:
  std::shared_ptr<Model> model;
  std::shared_ptr<Texture> texture;

  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 scale;
};

#endif
