// Joseph Arhar

#include "GameState.h"

GameState::GameState(std::shared_ptr<Level> level, std::shared_ptr<GameCamera> camera) {
  this->level = level;
  this->camera = camera;
  done = false;
}

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

void GameState::SetDone(bool done) {
  this->done = done;
}

bool GameState::Done() {
  return done;
}
