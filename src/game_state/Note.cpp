#include "Note.h"

#include <memory>
#include <iostream>

namespace gameobject {

Note::Note(glm::vec3 position,
           glm::vec3 scale,
           glm::vec3 rotation_axis,
           float rotation_angle,
           bool collected)
    : Collectible(NOTE_MESH,
                  collected,
                  position,
                  rotation_axis,
                  rotation_angle,
                  scale) {}

Note::~Note() {}

SecondaryType Note::GetSecondaryType() {
  return SecondaryType::NOTE;
}
}
