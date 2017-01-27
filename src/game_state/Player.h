// Joseph Arhar

#ifndef PLAYER_H_
#define PLAYER_H_

#include "game_state/GameObject.h"

class Player : public GameObject {
 public:
  // Represents change in velocity per tick
  static const float GRAVITY;

  Player(std::shared_ptr<Shape> model);
  ~Player();

  float GetVerticalVelocity();

  void SetVerticalVelocity(float vertical_velocity);

 private:
  float vertical_velocity;
};

#endif
