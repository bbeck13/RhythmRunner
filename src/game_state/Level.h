// bnbeck
#ifndef LEVEL_H_
#define LEVEL_H_

#include <memory>
#include <SFML/Audio.hpp>
#include <aquila/global.h>
#include <aquila/source/WaveFile.h>
#include <aquila/tools/TextPlot.h>
#include <aquila/transform/AquilaFft.h>
#include <aquila/transform/OouraFft.h>
#include <aquila/source/generator/SineGenerator.h>
#include <aquila/transform/FftFactory.h>

#include "GameObject.h"
#include "Platform.h"
#include "Note.h"
#include "Collectible.h"
#include "Obstacle.h"
#include "Octree.h"

class Level {
 public:
  Level(std::shared_ptr<sf::Music> music,
        std::shared_ptr<Octree> tree,
        std::shared_ptr<std::vector<Aquila::SignalSource>> sources);
  ~Level();

  std::shared_ptr<sf::Music> getMusic();
  std::shared_ptr<Octree> getTree();
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> getObjects();

  void SetTree(std::shared_ptr<Octree> tree);
  void AddItem(std::shared_ptr<GameObject> object);
  void RemoveItem(std::shared_ptr<GameObject> object);

 private:
  std::shared_ptr<sf::Music> music;
  std::shared_ptr<Octree> tree;
  std::shared_ptr<std::vector<Aquila::SignalSource>> sources;
};

#endif
