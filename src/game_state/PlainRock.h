// Alex Ottoboni

#ifndef __PLAINROCK_H__
#define __PLAINROCK_H__
#include "GameObject.h"
#include "Shape.h"
#include <glm/ext.hpp>

#define PLAIN_ROCK_MESH "models/plain_rock.obj"

namespace gameobject {
class PlainRock : public GameObject {
 public:
  PlainRock();
  PlainRock(glm::vec3 position, glm::vec3 scale);
  PlainRock(glm::vec3 position,
            glm::vec3 scale,
            glm::vec3 rotation_axis,
            float rotaiton_angle);
  ~PlainRock();

  std::shared_ptr<Shape> GetModel() override;
  ObjectType GetType() override;
  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Shape> shape;
  static bool isInitialized;
};
}
#endif
