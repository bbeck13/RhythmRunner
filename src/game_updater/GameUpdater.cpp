#include "GameUpdater.h"

GameUpdater::GameUpdater() {
  this->levelUpdater = std::make_shared<LevelUpdater>();
}
GameUpdater::~GameUpdater() {}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  levelUpdater->Update(game_state->GetLevel());
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  levelUpdater->Reset(game_state->GetLevel());
}
