// Joseph Arhar

#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <memory>

#include <glm/glm.hpp>

#include "Shape.h"
#include "Texture.h"

// GameObjects are 3D entities which can be rendered
class GameObject {
  public:
   GameObject(std::shared_ptr<Shape> model);
   virtual ~GameObject();

   std::shared_ptr<Shape> GetModel();

   glm::vec3 GetPosition();
   glm::vec3 GetDirection();
   float GetScale();
   std::shared_ptr<Texture> GetTexture(); // Texture may be null

   void SetPosition(glm::vec3 position);
   void SetDirection(glm::vec3 direction);
   void SetScale(float scale);
   void SetTexture(std::shared_ptr<Texture> texture);

  private:
   std::shared_ptr<Shape> model;
   std::shared_ptr<Texture> texture;

   glm::vec3 position;
   glm::vec3 direction;
   float scale;
};

#endif
