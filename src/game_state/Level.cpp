#include "Level.h"

Level::Level(std::shared_ptr<sf::Music> music,
             std::shared_ptr<std::vector<std::shared_ptr<Platform>>> platforms,
             std::shared_ptr<std::vector<std::shared_ptr<Note>>> notes) {
  this->music = music;
  this->platforms = platforms;
  this->notes = notes;
}

Level::~Level() {}

std::shared_ptr<sf::Music> Level::getMusic() {
  return music;
}

std::shared_ptr<std::vector<std::shared_ptr<Platform>>> Level::getPlatforms() {
  return platforms;
}

std::shared_ptr<std::vector<std::shared_ptr<Note>>> Level::getNotes() {
  return notes;
}
