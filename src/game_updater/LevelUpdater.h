// bnbeck

#ifndef LEVEL_UPDATER_H_
#define LEVEL_UPDATER_H_

#include <SFML/Audio.hpp>
#include "GameUpdater.h"
#include "Level.h"

class LevelUpdater : GameUpdater {
 public:
  LevelUpdater();
  ~LevelUpdater();

  void Update(std::shared_ptr<Level> levelState);
 private:
};

#endif
