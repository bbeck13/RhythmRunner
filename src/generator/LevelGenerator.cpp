// bnbeck
#include <iostream>
#include <cstdlib>
#include <time.h>

#include "LevelGenerator.h"
#include "Note.h"
#include "Level.h"
#include "Octree.h"
#include "MovingPlatform.h"
#include "DroppingPlatform.h"

#define COLLECT 3.2f
#define EPISILON 0.05f

std::pair<double, double> LevelGenerator::sizeRange(2.6f, 8.0f);

namespace {
template <typename tVal>
tVal mapRange(std::pair<tVal, tVal> a, std::pair<tVal, tVal> b, tVal inVal) {
  tVal inValNorm = inVal - a.first;
  tVal aUpperNorm = a.second - a.first;
  tVal normPosition = inValNorm / aUpperNorm;

  tVal bUpperNorm = b.second - b.first;
  tVal bValNorm = normPosition * bUpperNorm;
  tVal outVal = b.first + bValNorm;

  return outVal;
}
}

LevelGenerator::LevelGenerator(std::string musicFile) {
  this->wav = std::make_shared<Aquila::WaveFile>(musicFile);
  loaded = false;

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

LevelGenerator::LevelGenerator(
    std::string musicFile,
    std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects)
    : LevelGenerator(musicFile) {
  loaded = true;
  this->level = objects;
}

LevelGenerator::~LevelGenerator() {}

std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>
LevelGenerator::Generate() {
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objs =
      std::make_shared<std::vector<std::shared_ptr<GameObject>>>();
  if (loaded) {
    objs = level;
  } else {
    std::srand(time(NULL));

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

    int num_platforms = wav->getAudioLength() / (float)MS_PER_PLATFORM;
    int samplesPerPlatform = MS_PER_PLATFORM * (wav->getSamplesCount() /
                                                (double)wav->getAudioLength());

    double xPos = -1, yPos = 2, zPos = -5, power = 0, lastPower = 0;
    int lastSample = samplesPerPlatform, ups = 0, downs = 0, wobble = 0,
        dropping = 0, moving = 0;

    double pregame_platform_width = DELTA_X_PER_SECOND * PREGAME_SECONDS;
    objs->push_back(std::make_shared<gameobject::Platform>(
        glm::vec3(xPos - (pregame_platform_width / 2), yPos, zPos),
        glm::vec3(pregame_platform_width, 1, 7)));

    for (int i = 1; i < num_platforms; i++) {
      std::vector<Aquila::SampleType> sample;
      for (int j = lastSample;
           j < (lastSample + samplesPerPlatform) && j < wav->getSamplesCount();
           j++) {
        sample.push_back(wav->sample(j));
      }

      lastSample += samplesPerPlatform;
      Aquila::SignalSource src(sample, wav->getSampleFrequency());
      power = mapRange(range, sizeRange, Aquila::power(src));
      double delta = power - lastPower;
      lastPower = power;
      if (std::abs(delta) > EPISILON) {
        if (delta > 0) {
          yPos += PLATFORM_Y_DELTA;
          if (downs == 1) {
            wobble++;
          } else {
            wobble = 0;
          }
          downs = 0;
          ups++;
        } else {
          yPos -= PLATFORM_Y_DELTA;
          if (ups == 1) {
            wobble++;
          } else {
            wobble = 0;
          }
          ups = 0;
          downs++;
        }
      }

      xPos += PLATFORM_X_DELTA;
      if (wobble == 2) {
        wobble = 0;
        objs->pop_back();
        objs->pop_back();
        if (objs->back()->GetSecondaryType() ==
            SecondaryType::DROPPING_PLATFORM) {
          objs->pop_back();
          objs->push_back(std::make_shared<gameobject::Platform>(
              glm::vec3(xPos - 2 * PLATFORM_X_DELTA, yPos, zPos),
              glm::vec3(power, .5f, 7.0f)));
        }
      }
      if (downs > 2 || ups > 2) {
        glm::vec3 xDelta = glm::vec3(PLATFORM_X_DELTA, -PLATFORM_Y_DELTA, 0.0f);
        std::vector<glm::vec3> path = std::vector<glm::vec3>();
        path.push_back(objs->at(objs->size() - 2)->GetPosition() + xDelta);
        path.push_back(objs->at(objs->size() - 1)->GetPosition() + 2 * xDelta);
        path.push_back(objs->at(objs->size() - 3)->GetPosition());
        objs->pop_back();
        objs->pop_back();
        objs->pop_back();

        objs->push_back(std::make_shared<gameobject::MovingPlatform>(
            path.at(2), path,
            mapRange(sizeRange, std::pair<double, double>(0.01, 0.1), power)));
        ups = downs = 0;
        moving = 1;
      } else if (moving == 1) {
        moving = 0;
      } else {
        if (std::abs(delta) > EPISILON) {
          if (objs->back()->GetSecondaryType() ==
              SecondaryType::DROPPING_PLATFORM) {
            yPos = objs->back()->GetPosition().y;
          }
          objs->push_back(std::make_shared<gameobject::Platform>(
              glm::vec3(xPos, yPos, zPos), glm::vec3(power, .5f, 7.0f)));
          dropping = 0;
        } else {
          objs->push_back(std::make_shared<gameobject::DroppingPlatform>(
              glm::vec3(xPos, yPos, zPos), glm::vec3(power, .5f, 7.0f)));
          dropping++;
        }
      }
      if (power > COLLECT) {
        objs->push_back(std::make_shared<gameobject::Note>(
            glm::vec3(xPos, yPos + power - .5, zPos)));
      }
    }
  }
  return objs;
}

std::shared_ptr<Level> LevelGenerator::generateLevel() {
#ifdef DEBUG
  std::cerr << "Generating octree..." << std::endl;
#endif
  std::shared_ptr<Octree> tree = std::make_shared<Octree>(Generate());
  std::shared_ptr<Level> level =
      std::make_shared<Level>(this->getMusic(), tree);
#ifdef DEBUG
  std::cerr << "Generated octree!!" << std::endl;
#endif

  return level;
}
