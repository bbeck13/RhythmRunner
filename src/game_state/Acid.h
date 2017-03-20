#ifndef __ACID_H__
#define __ACID_H__

#include <glm/ext.hpp>

#include "Collectible.h"
#include "Program.h"

#define ACID_MESH "models/pill.obj"

namespace gameobject {
class Acid : public Collectible {
 public:
  static const glm::vec3 color;

  Acid(glm::vec3 position = glm::vec3(),
       glm::vec3 scale = glm::vec3(0.5, 0.5, 0.5),
       glm::vec3 rotation_axis = glm::vec3(0, 1, 0),
       float rotation_angle = 0,
       bool collected = false);
  virtual ~Acid();

  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Program> note_prog;
};
}
#endif
