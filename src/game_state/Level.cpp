#include "Level.h"
#include <iostream>

Level::Level(std::shared_ptr<sf::Music> music,
             std::shared_ptr<Octree> tree,
             std::shared_ptr<std::vector<Aquila::SignalSource>> sources,
             std::pair<double, double> range)
    : music(music), tree(tree), sources(sources), range(range) {}

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
  tree->insert(object);
}

void Level::RemoveItem(std::shared_ptr<GameObject> object) {
  tree->remove(object);
}

void Level::SetTree(std::shared_ptr<Octree> tree) {
  this->tree = tree;
}

double Level::GetPower(double progress) {
  return mapRange(
      range, std::make_pair(4.0f, 10.0f),
      Aquila::power(sources->at((int)(((double)sources->size()) * progress))));
}

double Level::mapRange(std::pair<double, double> a,
                       std::pair<double, double> b,
                       double inVal) {
  double inValNorm = inVal - a.first;
  double aUpperNorm = a.second - a.first;
  double normPosition = inValNorm / aUpperNorm;

  double bUpperNorm = b.second - b.first;
  double bValNorm = normPosition * bUpperNorm;
  double outVal = b.first + bValNorm;

  return outVal;
}
