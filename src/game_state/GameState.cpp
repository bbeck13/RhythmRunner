// Joseph Arhar

#include "GameState.h"

GameState::GameState() {}

GameState::~GameState() {}

std::shared_ptr<std::vector<GameObject>> GameState::GetGameObjects() {
  return game_objects;
}
