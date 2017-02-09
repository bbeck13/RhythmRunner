// Joseph Arhar

#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"
#include "AxisAlignedBox.h"

enum ObjectType { OBSTACLE, COLLECTIBLE, PLAYER };

// GameObjects are 3D entities which can be rendered
class GameObject {
 public:
  GameObject(std::shared_ptr<Shape> model);
  virtual ~GameObject();

  virtual std::shared_ptr<Shape> GetModel() = 0;
  virtual ObjectType GetType() = 0;

  glm::vec3 GetPosition();
  glm::vec3 GetRotationAxis();
  float GetRotationAngle();
  glm::vec3 GetScale();
  std::shared_ptr<Texture> GetTexture();  // Texture may be null
  MatrixStack GetTransform();
  AxisAlignedBox GetBoundingBox();

  void SetPosition(glm::vec3 position);
  void SetRotationAxis(glm::vec3 rotation_axis);
  void SetRotationAngle(float rotaiton_angle);
  void SetScale(glm::vec3 scale);
  void SetTexture(std::shared_ptr<Texture> texture);

 protected:
  std::shared_ptr<Shape> model;
  std::shared_ptr<Texture> texture;

  glm::vec3 position;
  glm::vec3 rotation_axis;
  float rotation_angle;
  glm::vec3 scale;
};

#endif
