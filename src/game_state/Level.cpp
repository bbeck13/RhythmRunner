#include "Level.h"

Level::Level(std::shared_ptr<sf::Music> music, std::shared_ptr<Octree> tree) {
  this->music = music;
  this->tree = tree;
}

Level::~Level() {}

std::shared_ptr<sf::Music> Level::getMusic() {
  return music;
}

std::shared_ptr<Octree> Level::getTree() {
  return tree;
}

std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> Level::getObjects() {
  return tree->getObjects();
}

void Level::AddItem(std::shared_ptr<GameObject> object) {
  this->tree->insert(object);
}
