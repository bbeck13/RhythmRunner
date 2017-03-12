#include "MoonRock.h"
#include <memory>
#include <iostream>

#include "GameRenderer.h"

namespace gameobject {

std::shared_ptr<Program> MoonRock::moon_rock_program;
std::shared_ptr<Texture> MoonRock::moon_rock_texture;

MoonRock::MoonRock(glm::vec3 position,
                   glm::vec3 scale,
                   float rotation_angle,
                   glm::vec3 rotation_axis)
    : GameObject(ROCK_MESH, position, rotation_axis, rotation_angle, scale) {
  if (!moon_rock_program) {
    moon_rock_program =
        GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/rock.json");
  }
  if (!moon_rock_texture) {
    moon_rock_texture =
        GameRenderer::TextureFromJSON(ASSET_DIR "/textures/rock.json");
  }
  program = moon_rock_program;
  texture = moon_rock_texture;
}

MoonRock::~MoonRock() {}

ObjectType MoonRock::GetType() {
  return ObjectType::OBSTACLE;
}

SecondaryType MoonRock::GetSecondaryType() {
  return SecondaryType::MOONROCK;
}
}
