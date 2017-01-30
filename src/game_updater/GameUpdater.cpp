#include "GameUpdater.h"

GameUpdater::GameUpdater() {
  this->levelUpdater = std::make_shared<LevelUpdater>();
  done = false;
}
GameUpdater::~GameUpdater() {}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  levelUpdater->Update(game_state->GetLevel());
  if (levelUpdater->Done()) {
    done = true;
    game_state->SetDone(done);
  }
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  levelUpdater->Reset(game_state->GetLevel());
  done = false;
  game_state->SetDone(done);
}
