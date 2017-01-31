// Joseph Arhar

#ifndef PLAYER_H_
#define PLAYER_H_

#include "game_state/GameObject.h"

#define PLAYER_MESH "../assets/models/bunny.obj"

class Player : public GameObject {
 public:
  // Represents change in velocity per tick
  static const float GRAVITY;

  Player(glm::vec3 position);
  ~Player();

  float GetVerticalVelocity();

  void SetVerticalVelocity(float vertical_velocity);

  static std::shared_ptr<Shape> GetShape();

 private:
  float vertical_velocity;
  static std::shared_ptr<Shape> shape;
  static bool isInitialized;
};

#endif
