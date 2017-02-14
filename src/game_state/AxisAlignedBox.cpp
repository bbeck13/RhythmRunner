// Joseph Arhar

#include "AxisAlignedBox.h"

#include <algorithm>
#include <glm/ext.hpp>

// static
bool AxisAlignedBox::IsColliding(const AxisAlignedBox& one,
                                 const AxisAlignedBox& two) {
  return one.min.x <= two.max.x && one.max.x >= two.min.x &&
         one.min.y <= two.max.y && one.max.y >= two.min.y &&
         one.min.z <= two.max.z && one.max.z >= two.min.z;
}

MatrixStack AxisAlignedBox::GetTransform(glm::vec3 position, glm::vec3 scale) {
  // TODO(jarhar): make this more efficient by caching calculated matrix
  MatrixStack transform;
  transform.pushMatrix();
  transform.loadIdentity();
  transform.translate(position);
  // TODO(jarhar): rotate
  transform.scale(scale);
  return transform;
}

AxisAlignedBox::AxisAlignedBox(std::shared_ptr<Shape> model,
                               glm::vec3 scale,
                               glm::vec3 position)
    : AxisAlignedBox(model, GetTransform(position, scale).topMatrix()) {}

AxisAlignedBox::AxisAlignedBox(glm::vec3 min, glm::vec3 max)
    : min(min), max(max) {}

AxisAlignedBox::AxisAlignedBox(std::shared_ptr<Shape> model,
                               glm::mat4 transform) {
  bool first = true;
  std::vector<float> pos_buf = model->GetPositions();
  for (int i = 0; i < pos_buf.size(); i += 3) {
    glm::vec4 position(pos_buf[i], pos_buf[i + 1], pos_buf[i + 2], 1.0f);
    glm::vec4 new_position = transform * position;
    if (first) {
      first = false;
      min.x = max.x = new_position.x;
      min.y = max.y = new_position.y;
      min.z = max.z = new_position.z;
    } else {
      max.x = std::max(max.x, new_position.x);
      min.x = std::min(min.x, new_position.x);
      max.y = std::max(max.y, new_position.y);
      min.y = std::min(min.y, new_position.y);
      max.z = std::max(max.z, new_position.z);
      min.z = std::min(min.z, new_position.z);
    }
  }
}

AxisAlignedBox::~AxisAlignedBox() {}

glm::vec3 AxisAlignedBox::GetMin() {
  return min;
}

glm::vec3 AxisAlignedBox::GetMax() {
  return max;
}

std::string AxisAlignedBox::ToString() {
  return "min: " + glm::to_string(GetMin()) + ", max: " +
         glm::to_string(GetMax());
}

AxisAlignedBox AxisAlignedBox::merge(AxisAlignedBox other) {
  glm::vec3 otherMin = other.GetMin();
  glm::vec3 otherMax = other.GetMax();

  return AxisAlignedBox(
      glm::vec3(std::min(min.x, otherMin.x), std::min(min.z, otherMin.z),
                std::min(min.z, otherMin.z)),
      glm::vec3(std::max(max.x, otherMax.x), std::max(max.z, otherMax.z),
                std::max(max.z, otherMax.z)));
}
