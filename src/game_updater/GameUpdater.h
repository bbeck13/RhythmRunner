// Joseph Arhar

#ifndef GAME_UPDATER_H_
#define GAME_UPDATER_H_

#include <memory>
#include <unordered_set>

#include "GameState.h"
#include "Octree.h"
#include "GameObject.h"

class GameUpdater {
 public:
  GameUpdater();
  ~GameUpdater();

  void Update(std::shared_ptr<GameState> game_state);
  void Reset(std::shared_ptr<GameState> game_state);
  void Init(std::shared_ptr<GameState> game_state);

 private:
  void UpdateLevel(std::shared_ptr<GameState> game_state);
  void UpdatePlayer(std::shared_ptr<GameState> game_state);
  void UpdateCamera(std::shared_ptr<GameState> game_state);
  static std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
  GetCollidingObjects(AxisAlignedBox primary_object,
                      std::shared_ptr<Octree> tree);
  void StopGame(std::shared_ptr<GameState> game_state);
};

#endif
