// Joseph Arhar

#include "GameState.h"

GameState::GameState(std::shared_ptr<Level> level,
                     std::shared_ptr<GameCamera> camera,
                     std::shared_ptr<Player> player)
    : level(level),
      camera(camera),
      player(player),
      done(false),
      elapsed_ticks(0),
      timing_start_tick(0),
      music_timing_mode(false) {}

GameState::~GameState() {}

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

bool GameState::GetMusicTimingMode() {
  return music_timing_mode;
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

void GameState::SetMusicTimingMode(bool music_timing_mode) {
  this->music_timing_mode = music_timing_mode;
}
