// Joseph Arhar

#ifndef GAME_UPDATER_H_
#define GAME_UPDATER_H_

#include <memory>

#include "GameState.h"

class GameUpdater {
 public:
  GameUpdater();
  ~GameUpdater();

  virtual void Update(std::shared_ptr<GameState> game_state) {}

 private:
};

#endif
