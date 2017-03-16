#include "DMT.h"

#include <memory>

#include "GameRenderer.h"

namespace gameobject {

const glm::vec3 DMT::color = glm::vec3(150.0 / 255.0, 0 / 255, 236.0 / 255.0);

std::shared_ptr<Program> DMT::note_prog;
DMT::DMT(glm::vec3 position,
         glm::vec3 scale,
         glm::vec3 rotation_axis,
         float rotation_angle,
         bool collected)
    : Collectible(DMT_MESH,
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

DMT::~DMT() {}

SecondaryType DMT::GetSecondaryType() {
  return SecondaryType::DMT;
}
}
