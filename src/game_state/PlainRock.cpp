#include "PlainRock.h"
#include <memory>
#include <iostream>

#include "GameRenderer.h"

namespace gameobject {

std::shared_ptr<Program> PlainRock::rock_program;
std::shared_ptr<Texture> PlainRock::rock_texture;

PlainRock::PlainRock(glm::vec3 position,
                     glm::vec3 scale,
                     float rotation_angle,
                     glm::vec3 rotation_axis)
    : GameObject(PLAIN_ROCK_MESH,
                 position,
                 rotation_axis,
                 rotation_angle,
                 scale) {
  if (!rock_program) {
    rock_program =
        GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/rock.json");
  }
  if (!rock_texture) {
    rock_texture =
        GameRenderer::TextureFromJSON(ASSET_DIR "/textures/rock.json");
  }
  program = rock_program;
  texture = rock_texture;
}

PlainRock::~PlainRock() {}

ObjectType PlainRock::GetType() {
  return ObjectType::OBSTACLE;
}

SecondaryType PlainRock::GetSecondaryType() {
  return SecondaryType::PLAINROCK;
}
}
