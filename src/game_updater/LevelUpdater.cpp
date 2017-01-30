#include "LevelUpdater.h"
#include <iostream>

LevelUpdater::LevelUpdater() {
  this->position = START;
}
LevelUpdater::~LevelUpdater() {}

void LevelUpdater::Update(std::shared_ptr<Level> level) {
  if (position == START) {
    position = PLAYING;
    level->getMusic()->play();
    level->getMusic()->setLoop(false);
  } else if (level->getMusic()->getStatus() == sf::Music::Status::Stopped) {
    position = END;
  }
}

void LevelUpdater::Reset(std::shared_ptr<Level> level) {
  position = START;
  level->getMusic()->stop();
}

bool LevelUpdater::Done() {
  return position == END;
}
