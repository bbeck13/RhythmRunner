// Joseph Arhar

#include "game_state/GameObject.h"

// TODO(jarhar): make this constructor require all fields
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

glm::vec3 GameObject::GetRotationAxis() {
  return rotation_axis;
}

float GameObject::GetRotationAngle() {
  return rotation_angle;
}

glm::vec3 GameObject::GetScale() {
  return scale;
}

glm::mat4 GameObject::GetTransform() {
  // TODO(jarhar): make this more efficient by caching calculated matrix
  // TODO fix
  // TODO TODO TODO
  /*glm::mat4 transform(1.0);
  transform *= glm::scale(glm::mat4(1.0), scale);
  transform *= glm::rotate(glm::mat4(1.0), rotation_angle, rotation_axis);
  transform *= glm::translate(glm::mat4(1.0), position);
  return transform;*/
  //return glm::translate(glm::mat4(1.0), position) * glm::scale(glm::mat4(1.0), scale);
  return glm::translate(glm::mat4(1.0), position);
}

AxisAlignedBox GameObject::GetBoundingBox() {
  // TODO(jarhar): make this more efficient by caching calculated box
  AxisAlignedBox box(GetModel(), GetTransform());
  return box;
}

void GameObject::SetPosition(glm::vec3 position) {
  this->position = position;
}

void GameObject::SetRotationAxis(glm::vec3 rotation_axis) {
  this->rotation_axis = rotation_axis;
}

void GameObject::SetRotationAngle(float rotation_angle) {
  this->rotation_angle = rotation_angle;
}

void GameObject::SetScale(glm::vec3 scale) {
  this->scale = scale;
}

void GameObject::SetTexture(std::shared_ptr<Texture> texture) {
  this->texture = texture;
}
