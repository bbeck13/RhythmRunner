// bnbeck
#ifndef LEVEL_H_
#define LEVEL_H_

#include <memory>
#include <SFML/Audio.hpp>

#include "GameObject.h"
#include "Platform.h"
#include "Note.h"
#include "Collectible.h"
#include "Obstacle.h"
#include "Octree.h"

class Level {
 public:
  Level(std::shared_ptr<sf::Music> music, std::shared_ptr<Octree> tree);
  ~Level();

  std::shared_ptr<sf::Music> getMusic();
  std::shared_ptr<Octree> getTree();
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> getObjects();

 private:
  std::shared_ptr<sf::Music> music;
  std::shared_ptr<Octree> tree;
};

#endif
