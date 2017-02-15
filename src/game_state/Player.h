// Joseph Arhar

#ifndef PLAYER_H_
#define PLAYER_H_

#include "game_state/GameObject.h"

#define PLAYER_MESH "models/body_of_bike.obj"

class Player : public GameObject {
 public:
  // Represents change in velocity per tick
  static const float PLATFORM_SPACING;
  // TODO(jarhar): consider removing initial position
  static const glm::vec3 INITIAL_POSITION;

  Player();
  ~Player();

  float GetVerticalVelocity();
  bool GetSpacebarDown();
  std::shared_ptr<GameObject> GetGround(); // null if no ground
  std::shared_ptr<Shape> GetModel() override;

  void SetVerticalVelocity(float vertical_velocity);
  void SetSpacebarDown(bool spacebar_down);
  void SetGround(std::shared_ptr<GameObject> ground);
  void RemoveGround();
  void SetScore(int score);
  int GetScore();

 private:
  static std::shared_ptr<Shape> shape;
  static bool isInitialized;

  std::shared_ptr<GameObject> ground;
  float vertical_velocity;
  bool spacebar_down;
  int score;
};

#endif
