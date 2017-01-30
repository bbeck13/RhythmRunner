// Joseph Arhar

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <memory>
#include <vector>

#include "GameObject.h"
#include "Level.h"
#include "GameCamera.h"

class GameState {
 public:
  GameState(std::shared_ptr<Level> level);
  ~GameState();

  void insertGameObject(GameObject obj) { game_objects->push_back(obj); }
  std::shared_ptr<std::vector<GameObject>> GetGameObjects();
  std::shared_ptr<Level> GetLevel();
  void SetLevel(std::shared_ptr<Level> level);
  std::shared_ptr<GameCamera> GetCamera();
  void SetCamera(std::shared_ptr<GameCamera> camera);

 private:
  std::shared_ptr<std::vector<GameObject>> game_objects;
  std::shared_ptr<Level> level;
  std::shared_ptr<GameCamera> camera;
};

#endif
