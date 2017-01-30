// Joseph Arhar

#include "GameObject.h"

GameObject::GameObject(std::shared_ptr<Model> model) : model(model) {}

GameObject::~GameObject() {}

std::shared_ptr<Model> GameObject::GetModel() {
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

glm::vec3 GameObject::GetScale() {
  return scale;
}

void GameObject::SetPosition(glm::vec3 position) {
  this->position = position;
}

void GameObject::SetDirection(glm::vec3 direction) {
  this->direction = direction;
}

void GameObject::SetScale(glm::vec3 scale) {
  this->scale = scale;
}

void GameObject::SetTexture(std::shared_ptr<Texture> texture) {
  this->texture = texture;
}
