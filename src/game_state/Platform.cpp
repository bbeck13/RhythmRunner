#include "Platform.h"

#include <memory>
#include <iostream>

#include "GameRenderer.h"

namespace gameobject {

std::shared_ptr<Program> Platform::platform_program;
std::shared_ptr<Texture> Platform::platform_texture;

Platform::Platform(glm::vec3 position,
                   glm::vec3 scale,
                   glm::vec3 rotation_axis,
                   float rotation_angle)
    : Obstacle(PLATFORM_MESH, position, rotation_axis, rotation_angle, scale) {
  if (!platform_program) {
    platform_program =
        GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/platform.json");
  }
  if (!platform_texture) {
    platform_texture =
        GameRenderer::TextureFromJSON(ASSET_DIR "/textures/lunarrock.json");
  }
  program = platform_program;
  texture = platform_texture;
}

Platform::~Platform() {}

SecondaryType Platform::GetSecondaryType() {
  return SecondaryType::PLATFORM;
}
}
