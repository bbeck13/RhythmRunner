#include "LevelUpdater.h"

LevelUpdater::LevelUpdater() {
  this->position = 0;
}
LevelUpdater::~LevelUpdater() {}

void LevelUpdater::Update(std::shared_ptr<Level> level) {
  if (position == 0) {
    position = 1;
    level->getMusic()->play();
    level->getMusic()->setLoop(false);
  }
}

void LevelUpdater::Reset(std::shared_ptr<Level> level) {
  position = 0;
  level->getMusic()->stop();
}
