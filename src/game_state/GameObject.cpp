// Joseph Arhar

#include "GameObject.h"

GameObject::GameObject(std::shared_ptr<Shape> model) : model(model) {}

GameObject::~GameObject() {}

std::shared_ptr<Shape> GameObject::GetModel() {
  return model;
}

std::shared_ptr<Texture> GameObject::GetTexture() {
  return texture;
}

glm::vec3 GameObject::GetPosition() {
  return position;
}

glm::vec3 GameObject::GetDirection() {
  return direction;
}

float GameObject::GetScale() {
  return scale;
}

void GameObject::SetPosition(glm::vec3 position) {
  this->position = position;
}

void GameObject::SetDirection(glm::vec3 direction) {
  this->direction = direction;
}

void GameObject::SetScale(float scale) {
  this->scale = scale;
}

void GameObject::SetTexture(std::shared_ptr<Texture> texture) {
  this->texture = texture;
}
