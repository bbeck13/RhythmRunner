// Joseph Arhar

#ifndef AXIS_ALIGNED_BOX_H_
#define AXIS_ALIGNED_BOX_H_

#include <memory>
#include <glm/glm.hpp>
#include <string>

#include "game_renderer/Shape.h"

class AxisAlignedBox {
 public:
  static bool IsColliding(const AxisAlignedBox& one,
                          const AxisAlignedBox& two);

  AxisAlignedBox(std::shared_ptr<Shape> model, glm::mat4 transform);
  ~AxisAlignedBox();

  glm::vec3 GetMin();
  glm::vec3 GetMax();
  std::string ToString();

 private:
  glm::vec3 min, max;
};

#endif
