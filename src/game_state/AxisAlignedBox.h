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
  static glm::mat4 GetTransform(glm::vec3 position,
                                glm::vec3 scale,
                                float angle,
                                glm::vec3 axis);

  AxisAlignedBox(std::shared_ptr<Shape> model, glm::mat4 transform);
  AxisAlignedBox(glm::vec3 min = glm::vec3(-1, -1, -1),
                 glm::vec3 max = glm::vec3(1, 1, 1));
  AxisAlignedBox(std::shared_ptr<Shape> model,
                 glm::vec3 scale,
                 glm::vec3 position,
                 float angle = 0.0f,
                 glm::vec3 axis = glm::vec3(0, 1, 0));
  ~AxisAlignedBox();

  AxisAlignedBox merge(AxisAlignedBox other);
  std::vector<AxisAlignedBox> split_oct();
  std::vector<AxisAlignedBox> split_quad();
  glm::vec3 GetMin();
  glm::vec3 GetMax();
  glm::vec3 GetCenter();
  std::string ToString();
  float Distance(AxisAlignedBox other);

 private:
  glm::vec3 min, max;
};

#endif
