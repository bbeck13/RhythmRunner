// Joseph Arhar

#include "GameState.h"

GameState::GameState(std::shared_ptr<Level> level,
                     std::shared_ptr<GameCamera> camera,
                     std::shared_ptr<Player> player,
                     std::shared_ptr<Sky> sky)
    : level(level),
      camera(camera),
      player(player),
      sky(sky),
      done(false),
      elapsed_ticks(0),
      timing_start_tick(0) {
  objectsInView =
      std::make_shared<std::unordered_set<std::shared_ptr<GameObject>>>();
}

GameState::~GameState() {}

std::shared_ptr<Sky> GameState::GetSky() {
  return this->sky;
}

std::shared_ptr<Level> GameState::GetLevel() {
  return this->level;
}

std::shared_ptr<GameCamera> GameState::GetCamera() {
  return this->camera;
}

std::shared_ptr<Player> GameState::GetPlayer() {
  return this->player;
}

bool GameState::Done() {
  return done;
}

uint64_t GameState::GetElapsedTicks() {
  return elapsed_ticks;
}

uint64_t GameState::GetTimingStartTick() {
  return timing_start_tick;
}

void GameState::SetLevel(std::shared_ptr<Level> level) {
  this->level = level;
}

void GameState::SetCamera(std::shared_ptr<GameCamera> camera) {
  this->camera = camera;
}

void GameState::SetDone(bool done) {
  this->done = done;
}

void GameState::IncrementTicks() {
  elapsed_ticks++;
}

void GameState::SetTimingStartTick() {
  timing_start_tick = elapsed_ticks;
}

void GameState::SetItemsInView(
    std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>> objects) {
  this->objectsInView = objects;
}

std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
GameState::GetObjectsInView() {
  return objectsInView;
}
