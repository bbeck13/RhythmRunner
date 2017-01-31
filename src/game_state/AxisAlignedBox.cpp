// Joseph Arhar

#include "game_state/AxisAlignedBox.h"

#include <algorithm>

AxisAlignedBox::AxisAlignedBox(std::shared_ptr<Shape> model,
                               glm::mat4 transform) {
  min_x = min_y = min_z = FLT_MAX;
  max_x = max_y = max_z = FLT_MIN;
  const std::vector<unsigned>& pos_buf = model->GetElements();
  for (int i = 0; i < pos_buf.size(); i += 3) {
    glm::vec4 position(pos_buf[i], pos_buf[i + 1], pos_buf[i + 2], 1.0f);
    glm::vec4 new_position = transform * position;
    max_x = std::max(max_x, new_position.x);
    min_x = std::min(min_x, new_position.x);
    max_y = std::max(max_y, new_position.y);
    min_y = std::min(min_y, new_position.y);
    max_z = std::max(max_z, new_position.z);
    min_z = std::min(min_z, new_position.z);
  }
}

AxisAlignedBox::~AxisAlignedBox() {}

// static
bool AxisAlignedBox::IsColliding(const AxisAlignedBox& one,
                                 const AxisAlignedBox& two) {
  return (one.min_x <= two.max_x && one.max_x >= two.min_x) &&
         (one.min_y <= two.max_y && one.max_y >= two.min_y) &&
         (one.min_z <= two.max_z && one.max_z >= two.min_z);
}
