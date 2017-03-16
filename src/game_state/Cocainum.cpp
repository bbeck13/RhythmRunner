#include "Cocainum.h"

#include <memory>

#include "GameRenderer.h"

namespace gameobject {

const glm::vec3 Cocainum::color = glm::vec3(236.0 / 255.0, 0, 83.0 / 255.0);
std::shared_ptr<Program> Cocainum::note_prog;

Cocainum::Cocainum(glm::vec3 position,
                   glm::vec3 scale,
                   glm::vec3 rotation_axis,
                   float rotation_angle,
                   bool collected)
    : Collectible(COCAINUM_MESH,
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

Cocainum::~Cocainum() {}

SecondaryType Cocainum::GetSecondaryType() {
  return SecondaryType::COCAINUM;
}
}
