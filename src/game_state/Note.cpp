#include "Note.h"

#include <memory>

#include "TimingConstants.h"

#define NOTE_ROTATION_PER_SECOND 6.0
#define NOTE_ROTATION_PER_TICK (NOTE_ROTATION_PER_SECOND * SECONDS_PER_TICK)

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

void Note::Animate() {
  // spin to win
  SetRotationAxis(glm::vec3(0, 1, 0)); // TODO(jarhar): fix this
  SetRotationAngle(GetRotationAngle() + NOTE_ROTATION_PER_TICK);
}

}
