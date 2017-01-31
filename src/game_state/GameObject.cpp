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

MatrixStack GameObject::GetTransform() {
  // TODO(jarhar): make this more efficient by caching calculated matrix
  MatrixStack transform;
  transform.pushMatrix();
  transform.loadIdentity();
  transform.translate(GetPosition());
  // TODO(jarhar): rotate
  transform.scale(GetScale());
  return transform;
}

AxisAlignedBox GameObject::GetBoundingBox() {
  // TODO(jarhar): make this more efficient by caching calculated box
  AxisAlignedBox box(GetModel(), GetTransform().topMatrix());
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
