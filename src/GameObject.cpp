#include "GameObject.h"

GameObject::GameObject(Graphics *graphics) {
   this->graphics = graphics;
}
GameObject::GameObject(Graphics *graphics, string mesh) {
   this->graphics = graphics;
}
GameObject::GameObject(Graphics *graphics, string mesh, string texture) {
   this->graphics = graphics;
}

GameObject::~GameObject() {
   if (graphics) {
      delete graphics;
   }
   if (shape) {
      delete shape;
   }
   if (texture) {
      delete texture;
   }
   if (hitbox) {
      delete hitbox;
   }
}
