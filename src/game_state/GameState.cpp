// Joseph Arhar

#include "GameState.h"

GameState::GameState() {}

GameState::~GameState() {}

std::shared_ptr<std::vector<GameObject>> GameState::GetGameObjects() {
  return game_objects;
}

std::shared_ptr<Level> GameState::GetLevel() {
  return this->level;
}

void GameState::SetLevel(std::shared_ptr<Level> level) {
  this->level = level;
}

std::shared_ptr<GameCamera> GameState::GetCamera() {
  return this->camera;
}

void GameState::SetCamera(std::shared_ptr<GameCamera> camera) {
  this->camera = camera;
}
