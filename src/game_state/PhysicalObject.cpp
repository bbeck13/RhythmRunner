// Joseph Arhar

#include "PhysicalObject.h"

#include "ShapeManager.h"

PhysicalObject::PhysicalObject(const std::string& shape_path,
                               glm::vec3 position,
                               glm::vec3 rotation_axis,
                               float rotation_angle,
                               glm::vec3 scale)
    : shape(ShapeManager::GetShape(shape_path)),
      position(position),
      rotation_axis(rotation_axis),
      rotation_angle(rotation_angle),
      scale(scale),
      bounding_box_dirty(true),
      bounding_box(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)),
      parent_object(nullptr) {}

PhysicalObject::~PhysicalObject() {}

glm::vec3 PhysicalObject::GetPosition() const {
  return position;
}

glm::vec3 PhysicalObject::GetRotationAxis() const {
  return rotation_axis;
}

float PhysicalObject::GetRotationAngle() const {
  return rotation_angle;
}

glm::mat4 PhysicalObject::GetRotationMatrix() const {
  return glm::rotate(glm::mat4(1.0), rotation_angle, rotation_axis);
}

glm::vec3 PhysicalObject::GetScale() const {
  return scale;
}

std::shared_ptr<Texture> PhysicalObject::GetTexture() const {
  return texture;
}

std::shared_ptr<Shape> PhysicalObject::GetModel() const {
  return shape;
}

glm::mat4 PhysicalObject::GetTransform() const {
  glm::mat4 this_transform =
      glm::mat4(1.0) * glm::translate(glm::mat4(1.0), position) *
      GetRotationMatrix() * glm::scale(glm::mat4(1.0), scale);

  // If this object is part of a hierarchy, apply the parent's transform
  if (parent_object) {
    this_transform = parent_object->GetTransform() * this_transform;
  }

  return this_transform;
}

AxisAlignedBox PhysicalObject::GetIndividualBoundingBox() {
  if (bounding_box_dirty || parent_object) {
    bounding_box = AxisAlignedBox(shape, GetTransform());
    bounding_box_dirty = false;
  }
  return bounding_box;
}

AxisAlignedBox PhysicalObject::GetBoundingBox() {
  AxisAlignedBox tree_bounding_box = GetIndividualBoundingBox();
  for (std::shared_ptr<PhysicalObject> sub_object : sub_objects) {
    tree_bounding_box = tree_bounding_box.merge(sub_object->GetBoundingBox());
  }
  return tree_bounding_box;
}

std::vector<std::shared_ptr<PhysicalObject>> PhysicalObject::GetSubObjects() {
  return sub_objects;
}

void PhysicalObject::SetPosition(glm::vec3 position) {
  this->position = position;
  bounding_box_dirty = true;
}

void PhysicalObject::SetRotationAxis(glm::vec3 rotation_axis) {
  this->rotation_axis = rotation_axis;
  bounding_box_dirty = true;
}

void PhysicalObject::SetRotationAngle(float rotation_angle) {
  this->rotation_angle = rotation_angle;
  bounding_box_dirty = true;
}

void PhysicalObject::SetScale(glm::vec3 scale) {
  this->scale = scale;
  bounding_box_dirty = true;
}

void PhysicalObject::SetTexture(std::shared_ptr<Texture> texture) {
  this->texture = texture;
}

void PhysicalObject::AddSubObject(std::shared_ptr<PhysicalObject> sub_object) {
  sub_object->parent_object = this;
  sub_objects.push_back(sub_object);
}
