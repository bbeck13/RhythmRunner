#ifndef __COCAINUM_H__
#define __COCAINUM_H__

#include <glm/ext.hpp>

#include "Collectible.h"
#include "Program.h"

#define COCAINUM_MESH "models/rock.obj"

namespace gameobject {
class Cocainum : public Collectible {
 public:
  static const glm::vec3 color;

  Cocainum(glm::vec3 position = glm::vec3(),
           glm::vec3 scale = glm::vec3(0.5, 0.5, 0.5),
           glm::vec3 rotation_axis = glm::vec3(0, 1, 0),
           float rotation_angle = 0,
           bool collected = false);
  virtual ~Cocainum();

  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Program> note_prog;
};
}
#endif
