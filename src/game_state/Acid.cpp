#include "Acid.h"

#include <memory>

#include "GameRenderer.h"

namespace gameobject {

const glm::vec3 Acid::color = glm::vec3(89.0 / 255.0, 236.0 / 255, 0);

std::shared_ptr<Program> Acid::note_prog;
Acid::Acid(glm::vec3 position,
           glm::vec3 scale,
           glm::vec3 rotation_axis,
           float rotation_angle,
           bool collected)
    : Collectible(ACID_MESH,
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

Acid::~Acid() {}

SecondaryType Acid::GetSecondaryType() {
  return SecondaryType::ACID;
}
}
