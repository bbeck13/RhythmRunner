// Joseph Arhar

#include "GameObject.h"

#include <assert.h>

#include "Logging.h"

// TODO(jarhar): make this constructor require all fields
GameObject::GameObject(std::shared_ptr<Shape> model) : model(model) {
  this->position = glm::vec3(0.0f, 0.0f, 0.0f);
  this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
  this->rotation_angle = 0.0f;
  this->rotation_axis = glm::vec3(0.0f, 1.0f, 0.0f);
}

GameObject::~GameObject() {}

std::shared_ptr<Texture> GameObject::GetTexture() {
  return texture;
}

glm::vec3 GameObject::GetPosition() const {
  return position;
}

glm::vec3 GameObject::GetRotationAxis() const {
  return rotation_axis;
}

float GameObject::GetRotationAngle() const {
  return rotation_angle;
}

glm::vec3 GameObject::GetScale() const {
  return scale;
}

MatrixStack GameObject::GetTransform() {
  return AxisAlignedBox::GetTransform(GetPosition(), GetScale(),
                                      GetRotationAngle(), GetRotationAxis());
}

AxisAlignedBox GameObject::GetBoundingBox() {
  // TODO(jarhar): make this more efficient by caching calculated box
  return AxisAlignedBox(GetModel(), GetTransform().topMatrix());
}

bool GameObject::Moves(SecondaryType type) {
  return type == SecondaryType::MOVING_PLATFORM ||
         type == SecondaryType::MONSTER;
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
