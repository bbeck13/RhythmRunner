#include "MovingPlatform.h"

#include "GameRenderer.h"

namespace gameobject {

std::shared_ptr<Program> MovingPlatform::platform_program;
std::shared_ptr<Texture> MovingPlatform::platform_texture;

MovingPlatform::MovingPlatform(glm::vec3 position, std::vector<glm::vec3> path)
    : MovingObject(path, position, 0.01f),
      Obstacle(MOVING_PLATFORM_MESH,
               position,
               glm::vec3(),
               0,
               glm::vec3(3, .6, 2.0f)) {
  if (!platform_program) {
    platform_program =
        GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/moving_plat.json");
  }
  if (!platform_texture) {
    platform_texture =
        GameRenderer::TextureFromJSON(ASSET_DIR "/textures/lunarrock.json");
  }
  program = platform_program;
  texture = platform_texture;
}

MovingPlatform::MovingPlatform(glm::vec3 position,
                               glm::vec3 scale,
                               glm::vec3 rotation_axis,
                               float rotation_angle,
                               glm::vec3 velocity,
                               std::vector<glm::vec3> path)
    : MovingObject(path, position, velocity),
      Obstacle(MOVING_PLATFORM_MESH,
               position,
               rotation_axis,
               rotation_angle,
               scale) {
  if (!platform_program) {
    platform_program =
        GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/moving_plat.json");
  }
  if (!platform_texture) {
    platform_texture =
        GameRenderer::TextureFromJSON(ASSET_DIR "/textures/lunarrock.json");
  }
  program = platform_program;
  texture = platform_texture;
}

MovingPlatform::MovingPlatform(glm::vec3 position,
                               std::vector<glm::vec3> path,
                               float velocity)
    : MovingObject(path, position, velocity),
      Obstacle(MOVING_PLATFORM_MESH,
               position,
               glm::vec3(),
               0,
               glm::vec3(3, .6, 2.0f)) {
  if (!platform_program) {
    platform_program =
        GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/moving_plat.json");
  }
  if (!platform_texture) {
    platform_texture =
        GameRenderer::TextureFromJSON(ASSET_DIR "/textures/lunarrock.json");
  }
  program = platform_program;
  texture = platform_texture;
}

MovingPlatform::~MovingPlatform() {}

SecondaryType MovingPlatform::GetSecondaryType() {
  return SecondaryType::MOVING_PLATFORM;
}
}
