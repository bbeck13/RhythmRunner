// Alex Ottoboni

#ifndef __MOONROCK_H__
#define __MOONROCK_H__
#include "GameObject.h"
#include "Shape.h"
#include <glm/ext.hpp>

#define ROCK_MESH "models/rock.obj"

namespace gameobject {
class MoonRock : public GameObject {
 public:
  MoonRock();
  MoonRock(glm::vec3 position, glm::vec3 scale);
  MoonRock(glm::vec3 position,
           glm::vec3 scale,
           glm::vec3 rotation_axis,
           float rotaiton_angle);
  ~MoonRock();

  std::shared_ptr<Shape> GetModel() override;
  ObjectType GetType() override;
  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Shape> shape;
  static bool isInitialized;
};
}
#endif
