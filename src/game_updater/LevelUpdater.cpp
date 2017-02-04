#include "LevelUpdater.h"

#include <iostream>

#include "LevelGenerator.h"

LevelUpdater::LevelUpdater() : position(START) {}

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

std::shared_ptr<Platform> LevelUpdater::CurrentPlatform(std::shared_ptr<Level> level) {
  return level->getPlatforms()->at(std::max(
      0l,
      std::min((long)(level->getMusic()->getPlayingOffset().asMilliseconds() /
                      (float)MS_PER_PLATFORM),
               (long)level->getPlatforms()->size() - 1)));
}

void LevelUpdater::Reset(std::shared_ptr<Level> level) {
  position = START;
  level->getMusic()->stop();
}

bool LevelUpdater::Done() {
  return position == END;
}
