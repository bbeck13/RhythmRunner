#include "Level.h"

Level::Level(std::shared_ptr<sf::Music> music,
             std::shared_ptr<std::vector<Platform>> level) {
  this->music = music;
  this->level = level;
}

Level::~Level() {}

std::shared_ptr<sf::Music> Level::getMusic() {
  return music;
}

std::shared_ptr<std::vector<Platform>> Level::getLevel() {
  return level;
}
