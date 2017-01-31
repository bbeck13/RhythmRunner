// Joseph Arhar

#include "GameState.h"

GameState::GameState(std::shared_ptr<Level> level,
                     std::shared_ptr<GameCamera> camera,
                     std::shared_ptr<Player> player)
    : level(level), camera(camera), player(player), done(false) {}

GameState::~GameState() {}

std::shared_ptr<Player> GameState::GetPlayer() {
  return this->player;
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
