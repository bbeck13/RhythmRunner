// Joseph Arhar

#ifndef GAME_UPDATER_H_
#define GAME_UPDATER_H_

#include <memory>

#include "GameState.h"
#include "LevelUpdater.h"

class GameUpdater {
 public:
  GameUpdater();
  ~GameUpdater();

  void Update(std::shared_ptr<GameState> game_state);
  void Reset(std::shared_ptr<GameState> game_state);
  void Done();

 private:
  std::shared_ptr<LevelUpdater> levelUpdater;
  bool done;
};

#endif
