// Joseph Arhar

#include "GameState.h"

#include <GLFW/glfw3.h>

#include "TimingConstants.h"

GameState::GameState(std::shared_ptr<Level> level,
                     std::shared_ptr<GameCamera> camera,
                     std::shared_ptr<Player> player)
    : level(level),
      camera(camera),
      player(player),
      done(false),
      elapsed_ticks(0),
      start_tick(0),
      start_time(0) {
  objectsInView =
      std::make_shared<std::unordered_set<std::shared_ptr<GameObject>>>();
  this->music_end_tick =
      level->getMusic()->getDuration().asMicroseconds() * TICKS_PER_MICRO +
      GetMusicStartTick();
}

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
  return done || elapsed_ticks > music_end_tick;
}

uint64_t GameState::GetElapsedTicks() {
  return elapsed_ticks;
}

uint64_t GameState::GetStartTick() {
  return start_tick;
}

uint64_t GameState::GetMusicStartTick() {
  // the music start tick is always dependent on the start tick
  return start_tick + PREGAME_SECONDS * TICKS_PER_SECOND;
}

double GameState::GetStartTime() {
  return start_time;
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

void GameState::SetStartTime() {
  elapsed_ticks = 0;
  start_tick = elapsed_ticks;
  start_time = glfwGetTime();
}

void GameState::SetItemsInView(
    std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>> objects) {
  this->objectsInView = objects;
}

std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
GameState::GetObjectsInView() {
  return objectsInView;
}
