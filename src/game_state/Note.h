// Alex Ottoboni

#ifndef __NOTE_H__
#define __NOTE_H__

#include <glm/ext.hpp>

#include "Collectible.h"

#define NOTE_MESH "models/note.obj"

namespace gameobject {
class Note : public Collectible {
 public:
  Note(glm::vec3 position = glm::vec3(),
       glm::vec3 scale = glm::vec3(0.5, 0.5, 0.5),
       glm::vec3 rotation_axis = glm::vec3(),
       float rotation_angle = 0,
       bool collected = false);
  virtual ~Note();

  SecondaryType GetSecondaryType() override;
};
}
#endif
