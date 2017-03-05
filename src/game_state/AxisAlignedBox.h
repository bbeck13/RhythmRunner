// Joseph Arhar

#ifndef AXIS_ALIGNED_BOX_H_
#define AXIS_ALIGNED_BOX_H_

#include <memory>
#include <glm/glm.hpp>
#include <string>
#include <glm/glm.hpp>

#include "MatrixStack.h"
#include "Shape.h"

class AxisAlignedBox {
 public:
  static bool IsColliding(const AxisAlignedBox& one, const AxisAlignedBox& two);
  static MatrixStack GetTransform(glm::vec3 position, glm::vec3 scale,
                                  float angle, glm::vec3 axis);

  AxisAlignedBox(std::shared_ptr<Shape> model, glm::mat4 transform);
  AxisAlignedBox(glm::vec3 min, glm::vec3 max);
  AxisAlignedBox(std::shared_ptr<Shape> model,
                 glm::vec3 scale,
                 glm::vec3 position);
  AxisAlignedBox(std::shared_ptr<Shape> model,
                 glm::vec3 scale,
                 glm::vec3 position,
                 float angle,
                 glm::vec3 axis);
  ~AxisAlignedBox();

  AxisAlignedBox merge(AxisAlignedBox other);
  glm::vec3 GetMin();
  glm::vec3 GetMax();
  std::string ToString();

 private:
  glm::vec3 min, max;
};

#endif
