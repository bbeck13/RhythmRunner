// Joseph Arhar

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <memory>
#include <vector>

#include "game_state/GameObject.h"

class GameState {
 public:
  GameState();
  ~GameState();

  std::unique_ptr<std::vector<GameObject>> GetGameObjects();

 private:
  std::unique_ptr<std::vector<GameObject>> game_objects;
};

#endif
