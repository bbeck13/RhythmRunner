// Joseph Arhar

#ifndef GAME_UPDATER_H_
#define GAME_UPDATER_H_

#include <memory>
#include <unordered_set>

#include "GameState.h"
#include "Octree.h"
#include "GameObject.h"
#include "PlayerUpdater.h"
#include "Particles.h"

class GameUpdater {
 public:
  GameUpdater();
  virtual ~GameUpdater();

  void Update(std::shared_ptr<GameState> game_state);
  void PostGameUpdate(std::shared_ptr<GameState> game_state);
  void Reset(std::shared_ptr<GameState> game_state);
  void Init(std::shared_ptr<GameState> game_state);
  uint64_t CalculateTargetTicks(std::shared_ptr<GameState> game_state);
<<<<<<< HEAD
  void UpdateCamera(std::shared_ptr<GameState> game_state);

=======
  ParticleGenerator *Particles;
    
>>>>>>> anguy particles
 private:
  void UpdateLevel(std::shared_ptr<GameState> game_state);
  void UpdateCamera(std::shared_ptr<GameState> game_state, bool update_with_player);

  PlayerUpdater player_updater;
};

#endif
