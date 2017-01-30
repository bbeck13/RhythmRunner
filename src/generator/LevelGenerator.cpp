// bnbeck
#include "LevelGenerator.h"
#include <iostream>

// each platform corresponds to 600 milliseconds .6 second
#define MS_PER_PLATFORM 600
#define Y_DELTA 0.4f
#define X_DELTA 3.8f
#define EPISILON 0.001f

LevelGenerator::LevelGenerator(std::string musicFile) {
  this->wav = std::make_shared<Aquila::WaveFile>(musicFile);

#ifdef DEBUG
  std::cerr << "Loaded file: " << wav->getFilename() << " ("
            << wav->getBitsPerSample() << "b)" << std::endl;
  std::cerr << wav->getSamplesCount() << " samples at " << wav->getAudioLength()
            << " ms" << std::endl;
#endif

  this->music = std::make_shared<sf::Music>();
  if (!music->openFromFile(musicFile)) {
    std::cerr << "Couldn't play " << musicFile << std::endl;
    exit(EXIT_FAILURE);
  }

#ifdef DEBUG
  std::cerr << "Loaded: " << musicFile << ":" << std::endl;
  std::cerr << " " << music->getDuration().asSeconds() << " seconds"
            << std::endl;
  std::cerr << " " << music->getSampleRate() << " samples / sec" << std::endl;
  std::cerr << " " << music->getChannelCount() << " channels" << std::endl;
#endif
}

LevelGenerator::~LevelGenerator() {}

std::shared_ptr<std::vector<Platform>> LevelGenerator::generateLevel() {
  std::shared_ptr<std::vector<Platform>> level =
      std::make_shared<std::vector<Platform>>();

#ifdef DEBUG
  std::cerr << "Generating level ...." << std::endl;
#endif
  std::pair<double, double> range;
  Aquila::SampleType maxValue = 0, minValue = 0;

  for (size_t j = 0; j < wav->getSamplesCount(); ++j) {
    std::vector<Aquila::SampleType> v = {wav->sample(j)};
    Aquila::SignalSource src(v, wav->getSampleFrequency());
    if (Aquila::power(src) > maxValue) {
      maxValue = Aquila::power(src);
    }
    if (Aquila::power(src) < minValue) {
      minValue = Aquila::power(src);
    }
  }
  range = std::pair<double, double>(minValue, maxValue);

  int platforms = wav->getAudioLength() / (float)MS_PER_PLATFORM;
  int samplesPerPlatform = MS_PER_PLATFORM * (wav->getSamplesCount() /
                                              (double)wav->getAudioLength());

  double xPos = -1, yPos = -1, zPos = -5, power = 0, lastPower = 0;
  int lastSample = samplesPerPlatform;
  level->push_back(Platform(glm::vec3(xPos, yPos, zPos)));
  for (int i = 1; i < platforms; i++) {
    std::vector<Aquila::SampleType> sample;
    for (int j = lastSample;
         j < (lastSample + samplesPerPlatform) && j < wav->getSamplesCount();
         j++) {
      sample.push_back(wav->sample(j));
    }
    Aquila::SignalSource src(sample, wav->getSampleFrequency());
    power = Aquila::power(src);
    double delta = power - lastPower;
    if (std::abs(delta) > EPISILON) {
      if (delta > 0) {
        yPos += Y_DELTA;
      } else {
        yPos -= Y_DELTA;
      }
    }
    xPos += X_DELTA;
    level->push_back(Platform(glm::vec3(xPos, yPos, zPos)));
  }
#ifdef DEBUG
  std::cerr << "Generated level!" << std::endl;
#endif

  return level;
}
