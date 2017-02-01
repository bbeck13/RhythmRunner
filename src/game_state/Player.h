// Joseph Arhar

#ifndef PLAYER_H_
#define PLAYER_H_

#include "game_state/GameObject.h"

#define PLAYER_MESH "../assets/models/bunny.obj"

class Player : public GameObject {
 public:
  // Represents change in velocity per tick
  static const float GRAVITY, JUMP_VELOCITY, PLATFORM_SPACING;

  Player(glm::vec3 position);
  ~Player();

  float GetVerticalVelocity();
  bool GetSpacebarDown();
  std::shared_ptr<GameObject> GetGround(); // null if no ground
  std::shared_ptr<Shape> GetModel() override;

  void SetVerticalVelocity(float vertical_velocity);
  void SetSpacebarDown(bool spacebar_down);
  void SetGround(std::shared_ptr<GameObject> ground);
  void RemoveGround();

 private:
  static std::shared_ptr<Shape> shape;
  static bool isInitialized;

  std::shared_ptr<GameObject> ground;
  float vertical_velocity;
  bool spacebar_down;
};

#endif
