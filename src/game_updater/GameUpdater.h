// Joseph Arhar

#ifndef GAME_UPDATER_H_
#define GAME_UPDATER_H_

#include <memory>

#include "GameState.h"

class GameUpdater {
 public:
  GameUpdater();
  ~GameUpdater();

  void Update(std::shared_ptr<GameState> game_state);
  void Reset(std::shared_ptr<GameState> game_state);
  void Init(std::shared_ptr<GameState> game_state);

 private:
  void UpdatePlayer(std::shared_ptr<GameState> game_state);
  void UpdateCamera(std::shared_ptr<GameState> game_state);
};

#endif
