// Joseph Arhar

#ifndef PLAYER_UPDATER_H_
#define PLAYER_UPDATER_H_

#include <memory>

#include "GameState.h"
#include "Player.h"

class PlayerUpdater {
 public:
  PlayerUpdater();
  virtual ~PlayerUpdater();

  AxisAlignedBox MovePlayer(std::shared_ptr<GameState> game_state);
  void AnimatePlayer(std::shared_ptr<GameState> game_state);
  void CollisionCheck(std::shared_ptr<GameState> game_state, AxisAlignedBox previous_player_box);
  void ChangeAnimation(std::shared_ptr<GameState> game_state,
                       Player::Animation new_animation);

 private:
  void Jump(std::shared_ptr<GameState> game_state);
  void Land(std::shared_ptr<GameState> game_state,
            std::shared_ptr<GameObject> ground);
  void SnapToGround(std::shared_ptr<GameState> game_state);
  void Death(std::shared_ptr<GameState> game_state);
};

#endif  // PLAYER_UPDATER_H_
