// bnbeck

#ifndef LEVEL_GENERATOR_H_
#define LEVEL_GENERATOR_H_

#include <string>
#include <memory>

#include <aquila/global.h>
#include <aquila/source/WaveFile.h>
#include <aquila/tools/TextPlot.h>
#include <aquila/transform/AquilaFft.h>
#include <aquila/transform/OouraFft.h>
#include <aquila/source/generator/SineGenerator.h>
#include <aquila/transform/FftFactory.h>
#include <SFML/Audio.hpp>

#include "Platform.h"

class LevelGenerator {
 public:
  LevelGenerator(std::string musicFile);
  ~LevelGenerator();

  std::shared_ptr<sf::Music> getMusic() { return music; }
  std::shared_ptr<std::vector<Platform>> generateLevel();

 private:
  std::shared_ptr<Aquila::WaveFile> wav;
  std::shared_ptr<sf::Music> music;
};

#endif
