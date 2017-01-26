// Joseph Arhar

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <memory>
#include <vector>

#include "GameObject.h"

class GameState {
 public:
  GameState();
  ~GameState();

  std::shared_ptr<std::vector<GameObject>> GetGameObjects();

 private:
  std::shared_ptr<std::vector<GameObject>> game_objects;
};

#endif
