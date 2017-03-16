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

glm::mat4 AxisAlignedBox::GetTransform(glm::vec3 position,
                                       glm::vec3 scale,
                                       float angle,
                                       glm::vec3 axis) {
  MatrixStack transform;
  transform.pushMatrix();
  transform.loadIdentity();
  transform.translate(position);
  transform.rotate(angle, axis);
  transform.scale(scale);
  return transform.topMatrix();
}

AxisAlignedBox::AxisAlignedBox(std::shared_ptr<Shape> model,
                               glm::vec3 scale,
                               glm::vec3 position,
                               float angle,
                               glm::vec3 axis)
    : AxisAlignedBox(model, GetTransform(position, scale, angle, axis)) {}

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

glm::vec3 AxisAlignedBox::GetCenter() {
  return glm::vec3(min.x + (max.x - min.x) / 2, min.y + (max.y - min.y) / 2,
                   min.z + (max.z - min.z) / 2);
}

std::string AxisAlignedBox::ToString() {
  return "min: " + glm::to_string(GetMin()) + ", max: " +
         glm::to_string(GetMax());
}

AxisAlignedBox AxisAlignedBox::merge(AxisAlignedBox other) {
  glm::vec3 otherMin = other.GetMin();
  glm::vec3 otherMax = other.GetMax();

  return AxisAlignedBox(
      glm::vec3(std::min(min.x, otherMin.x), std::min(min.y, otherMin.y),
                std::min(min.z, otherMin.z)),
      glm::vec3(std::max(max.x, otherMax.x), std::max(max.y, otherMax.y),
                std::max(max.z, otherMax.z)));
}

float AxisAlignedBox::Distance(AxisAlignedBox other) {
  if (IsColliding(*this, other)) {
    return 0;
  }
  return distance(this->GetCenter(), other.GetCenter());
}

std::vector<AxisAlignedBox> AxisAlignedBox::split_quad() {
  std::vector<AxisAlignedBox> quadrants;

  // mid
  glm::vec3 mid_top_mid(0.5f * (GetMin().x + GetMax().x), GetMax().y,
                        0.5f * (GetMin().z + GetMax().z));
  glm::vec3 right_top_mid(GetMax().x, GetMax().y,
                          0.5f * (GetMin().z + GetMax().z));
  glm::vec3 left_bottom_mid(GetMin().x, GetMin().y,
                            0.5f * (GetMin().z + GetMax().z));
  glm::vec3 mid_bottom_mid(0.5f * (GetMin().x + GetMax().x), GetMin().y,
                           0.5f * (GetMin().z + GetMax().z));
  // front
  glm::vec3 mid_bottom_front(0.5f * (GetMin().x + GetMax().x), GetMin().y,
                             GetMin().z);
  // back
  glm::vec3 mid_top_back(0.5f * (GetMin().x + GetMax().x), GetMax().y,
                         GetMax().z);

  quadrants.push_back(AxisAlignedBox(GetMin(), mid_top_mid));
  quadrants.push_back(AxisAlignedBox(mid_bottom_front, right_top_mid));

  quadrants.push_back(AxisAlignedBox(left_bottom_mid, mid_top_back));
  quadrants.push_back(AxisAlignedBox(mid_bottom_mid, GetMax()));

  return quadrants;
}

std::vector<AxisAlignedBox> AxisAlignedBox::split_oct() {
  std::vector<AxisAlignedBox> quadrants;

  // front points
  glm::vec3 mid_bottom_front(0.5f * (GetMin().x + GetMax().x), GetMin().y,
                             GetMin().z);
  glm::vec3 left_mid_front(GetMin().x, 0.5f * (GetMin().y + GetMax().y),
                           GetMin().z);
  glm::vec3 mid_mid_front(0.5f * (GetMin().x + GetMax().x),
                          0.5f * (GetMin().y + GetMax().y), GetMin().z);
  // middle points
  glm::vec3 left_bottom_mid(GetMin().x, GetMin().y,
                            0.5f * (GetMin().z + GetMax().z));
  glm::vec3 mid_bottom_mid(0.5f * (GetMin().x + GetMax().x), GetMin().y,
                           0.5f * (GetMin().z + GetMax().z));
  glm::vec3 left_mid_mid(GetMin().x, 0.5f * (GetMin().y + GetMax().y),
                         0.5f * (GetMin().z + GetMax().z));
  glm::vec3 right_mid_mid(GetMax().x, 0.5f * (GetMin().y + GetMax().y),
                          0.5f * (GetMin().z + GetMax().z));
  glm::vec3 mid_mid_mid(0.5f * (GetMin().x + GetMax().x),
                        0.5f * (GetMin().y + GetMax().y),
                        0.5f * (GetMin().z + GetMax().z));
  glm::vec3 mid_top_mid(0.5f * (GetMin().x + GetMax().x), GetMax().y,
                        0.5f * (GetMin().z + GetMax().z));
  glm::vec3 right_top_mid(GetMax().x, GetMax().y,
                          0.5f * (GetMin().z + GetMax().z));
  // far points
  glm::vec3 mid_mid_back(0.5f * (GetMin().x + GetMax().x),
                         0.5f * (GetMin().y + GetMax().y), GetMax().z);
  glm::vec3 mid_top_back(0.5f * (GetMin().x + GetMax().x), GetMax().y,
                         GetMax().z);
  glm::vec3 right_mid_back(GetMax().x, 0.5f * (GetMin().y + GetMax().y),
                           GetMax().z);

  // front quadrants
  quadrants.push_back(AxisAlignedBox(GetMin(), mid_mid_mid));
  quadrants.push_back(AxisAlignedBox(mid_bottom_front, right_mid_mid));
  quadrants.push_back(AxisAlignedBox(left_mid_front, mid_top_mid));
  quadrants.push_back(AxisAlignedBox(mid_mid_front, right_top_mid));

  // back quadrants
  quadrants.push_back(AxisAlignedBox(left_bottom_mid, mid_mid_back));
  quadrants.push_back(AxisAlignedBox(mid_bottom_mid, right_mid_back));
  quadrants.push_back(AxisAlignedBox(left_mid_mid, mid_top_back));
  quadrants.push_back(AxisAlignedBox(mid_mid_mid, GetMax()));

  return quadrants;
}
