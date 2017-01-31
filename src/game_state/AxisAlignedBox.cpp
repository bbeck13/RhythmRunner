// Joseph Arhar

#include "game_state/AxisAlignedBox.h"

#include <algorithm>

// static
bool AxisAlignedBox::IsColliding(const AxisAlignedBox& one,
                                 const AxisAlignedBox& two) {
  return one.min.x <= two.max.x && one.max.x >= two.min.x &&
         one.min.y <= two.max.y && one.max.y >= two.min.y &&
         one.min.z <= two.max.z && one.max.z >= two.min.z;
}

AxisAlignedBox::AxisAlignedBox(std::shared_ptr<Shape> model,
                               glm::mat4 transform) {
  min.x = min.y = min.z = FLT_MAX;
  max.x = max.y = max.z = FLT_MIN;
  const std::vector<unsigned>& pos_buf = model->GetElements();
  for (int i = 0; i < pos_buf.size(); i += 3) {
    glm::vec4 position(pos_buf[i], pos_buf[i + 1], pos_buf[i + 2], 1.0f);
    // TODO(jarhar): should we consider what happens to the 4th coordinate
    // after the transform?
    glm::vec4 new_position = transform * position;
    max.x = std::max(max.x, new_position.x);
    min.x = std::min(min.x, new_position.x);
    max.y = std::max(max.y, new_position.y);
    min.y = std::min(min.y, new_position.y);
    max.z = std::max(max.z, new_position.z);
    min.z = std::min(min.z, new_position.z);
  }
}

AxisAlignedBox::~AxisAlignedBox() {}

glm::vec3 AxisAlignedBox::GetMin() {
  return min;
}

glm::vec3 AxisAlignedBox::GetMax() {
  return max;
}