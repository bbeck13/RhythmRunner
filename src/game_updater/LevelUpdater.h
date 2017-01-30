// bnbeck

#ifndef LEVEL_UPDATER_H_
#define LEVEL_UPDATER_H_

#include <SFML/Audio.hpp>
#include "Level.h"

class LevelUpdater {
 public:
  LevelUpdater();
  ~LevelUpdater();

  void Update(std::shared_ptr<Level> levelState);
  void Reset(std::shared_ptr<Level> level);

 private:
  double position;
};

#endif
