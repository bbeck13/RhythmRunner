#include "GameUpdater.h"
#include <iostream>

GameUpdater::GameUpdater() {
  this->levelUpdater = std::make_shared<LevelUpdater>();
  done = false;
}
GameUpdater::~GameUpdater() {}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  levelUpdater->Update(game_state->GetLevel());
 
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  camera->setPosition(glm::vec3(camera->getPosition()[0] + 0.05, camera->getPosition()[1], camera->getPosition()[2]));
  camera->setLookAt(glm::vec3(camera->getLookAt()[0] + 0.05, camera->getLookAt()[1], camera->getLookAt()[2]));

  std::cout << " " << camera->getPosition()[0] << std::endl;
 
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
