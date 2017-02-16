// Joseph Arhar

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <memory>
#include <vector>
#include <unordered_set>

#include "GameObject.h"
#include "Level.h"
#include "GameCamera.h"
#include "Player.h"

class GameState {
 public:
  GameState(std::shared_ptr<Level> level,
            std::shared_ptr<GameCamera> camera,
            std::shared_ptr<Player> player);
  ~GameState();

  std::shared_ptr<Level> GetLevel();
  std::shared_ptr<GameCamera> GetCamera();
  std::shared_ptr<Player> GetPlayer();
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
  GetObjectsInView();

  bool Done();
  uint64_t GetElapsedTicks();
  uint64_t GetTimingStartTick();

  void SetLevel(std::shared_ptr<Level> level);
  void SetCamera(std::shared_ptr<GameCamera> camera);
  void SetDone(bool done);
  void IncrementTicks();
  void SetTimingStartTick();
  void SetItemsInView(
      std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>> objects);

 private:
  std::shared_ptr<Level> level;
  std::shared_ptr<GameCamera> camera;
  std::shared_ptr<Player> player;
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
      objectsInView;
  bool done;
  uint64_t elapsed_ticks;
  uint64_t timing_start_tick;
};

#endif
