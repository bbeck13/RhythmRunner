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

// each platform corresponds to 600 milliseconds .6 second
#define MS_PER_PLATFORM 900
#define Y_DELTA 0.4f
#define X_DELTA 6.0f
#define EPISILON 0.0001f

class LevelGenerator {
 public:
  LevelGenerator(std::string musicFile);
  ~LevelGenerator();

  std::shared_ptr<sf::Music> getMusic() { return music; }
  std::shared_ptr<std::vector<std::shared_ptr<Platform>>> generateLevel();

 private:
  std::shared_ptr<Aquila::WaveFile> wav;
  std::shared_ptr<sf::Music> music;
};

#endif
