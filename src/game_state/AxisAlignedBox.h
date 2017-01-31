// Joseph Arhar

#ifndef AXIS_ALIGNED_BOX_H_
#define AXIS_ALIGNED_BOX_H_

#include <memory>
#include <glm/glm.hpp>

#include "game_renderer/Shape.h"

class AxisAlignedBox {
 public:
  AxisAlignedBox(std::shared_ptr<Shape> model, glm::mat4 transform);
  ~AxisAlignedBox();

  static bool IsColliding(const AxisAlignedBox& one,
                          const AxisAlignedBox& two);

 private:
  float min_x, max_x;
  float min_y, max_y;
  float min_z, max_z;
};

#endif
