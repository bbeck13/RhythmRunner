#include "DroppingPlatform.h"

#include "GameRenderer.h"
#include "TimingConstants.h"

namespace gameobject {

std::shared_ptr<Program> DroppingPlatform::platform_program_down;
std::shared_ptr<Program> DroppingPlatform::platform_program_up;
std::shared_ptr<Texture> DroppingPlatform::platform_texture;

DroppingPlatform::DroppingPlatform(glm::vec3 position,
                                   glm::vec3 scale,
                                   glm::vec3 rotation_axis,
                                   float rotation_angle,
                                   float dropVel,
                                   bool dropping)
    : Obstacle(PLATFORM_SHAPE, position, rotation_axis, rotation_angle, scale),
      dropVel(dropVel),
      dropping(dropping),
      originalPosition(position) {
  if (!platform_program_up) {
    platform_program_up = GameRenderer::ProgramFromJSON(
        ASSET_DIR "/shaders/dropping_plat_up.json");
  }
  if (!platform_program_down) {
    platform_program_down = GameRenderer::ProgramFromJSON(
        ASSET_DIR "/shaders/dropping_plat_down.json");
  }
  if (!platform_texture) {
    platform_texture =
        GameRenderer::TextureFromJSON(ASSET_DIR "/textures/lunarrock.json");
  }
  if (dropVel < 0) {
    program = platform_program_down;
  } else {
    program = platform_program_up;
  }
  texture = platform_texture;
}

void DroppingPlatform::SetDropping() {
  dropping = true;
}

float DroppingPlatform::GetYVelocity() const {
  return dropVel;
}

void DroppingPlatform::Reset() {
  SetPosition(originalPosition);
  dropping = false;
}

bool DroppingPlatform::IsDropping() const {
  return dropping;
}

SecondaryType DroppingPlatform::GetSecondaryType() {
  return program == platform_program_down
             ? SecondaryType::DROPPING_PLATFORM_DOWN
             : SecondaryType::DROPPING_PLATFORM_UP;
}

AxisAlignedBox DroppingPlatform::GetFullBox() {
  AxisAlignedBox box(shape, scale, position, rotation_angle, rotation_axis);
  glm::vec3 pos =
      position +
      glm::vec3(0.0f, dropVel * std::ceill((box.GetMax().x - box.GetMin().x) /
                                           DELTA_X_PER_TICK),
                0.0f);
  box = box.merge(
      AxisAlignedBox(shape, scale, pos, rotation_angle, rotation_axis));
  return box;
}
}
