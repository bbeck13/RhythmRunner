#include "Sky.h"

#include <memory>
#include <iostream>

#include "Program.h"
#include "Texture.h"
#include "GameRenderer.h"

Sky::Sky(glm::vec3 position, glm::vec3 scale)
    : GameObject(SKY_MESH, position, glm::vec3(1, 0, 0), 0, scale) {
  program = std::make_shared<Program>();
  texture = std::make_shared<Texture>();

  program = GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/sky.json");
  texture = GameRenderer::TextureFromJSON(ASSET_DIR "/textures/nightsky.json");
}

Sky::~Sky() {}

ObjectType Sky::GetType() {
  return ObjectType::SCENERY;
}

SecondaryType Sky::GetSecondaryType() {
  return SecondaryType::SKY;
}
