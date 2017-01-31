// bnbeck

#ifndef LEVEL_UPDATER_H_
#define LEVEL_UPDATER_H_

#include <SFML/Audio.hpp>
#include "Level.h"
#include "Platform.h"

#define START 0
#define PLAYING 1
#define END 2

#define START 0
#define PLAYING 1
#define END 2

class LevelUpdater {
 public:
  LevelUpdater();
  ~LevelUpdater();

  void Update(std::shared_ptr<Level> levelState);
  void Reset(std::shared_ptr<Level> level);
  // CurrentPlatform is used for camera manipulation only
  Platform CurrentPlatform(std::shared_ptr<Level> level);
  bool Done();

 private:
  double position;
};

#endif
