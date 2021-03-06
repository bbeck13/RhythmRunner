#include "Note.h"

#include <memory>

#include "TimingConstants.h"
#include "GameRenderer.h"

namespace gameobject {

std::shared_ptr<Program> Note::note_prog;
Note::Note(glm::vec3 position,
           glm::vec3 scale,
           glm::vec3 rotation_axis,
           float rotation_angle,
           bool collected)
    : Collectible(NOTE_MESH,
                  collected,
                  0,
                  position,
                  rotation_axis,
                  rotation_angle,
                  scale) {
  if (!note_prog) {
    note_prog = GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/note.json");
  }
  program = note_prog;
}

Note::~Note() {}

SecondaryType Note::GetSecondaryType() {
  return SecondaryType::NOTE;
}
}
