// Joseph Arhar

#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"

#define PLAYER_MESH "models/body_of_bike.obj"

class Player : public GameObject {
 public:
  // Represents change in velocity per tick
  static const float PLATFORM_SPACING;
  // TODO(jarhar): consider removing initial position
  static const glm::vec3 INITIAL_POSITION;

  Player();
  ~Player();

  void Reset();
  float GetYVelocity();
  float GetZVelocity();
  std::shared_ptr<GameObject> GetGround(); // null if no ground
  std::shared_ptr<Shape> GetModel() override;
  ObjectType GetType() override;
  SecondaryType GetSecondaryType() override;

  void SetYVelocity(float y_velocity);
  void SetGround(std::shared_ptr<GameObject> ground);
  void RemoveGround();
  void SetScore(int score);
  void SetZVelocity(float z_velocity);
  int GetScore();

 private:
  static std::shared_ptr<Shape> shape;
  static bool isInitialized;

  std::shared_ptr<GameObject> ground;
  float y_velocity;
  float z_velocity;
  int score;
};

#endif
