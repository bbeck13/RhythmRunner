// Joseph Arhar

#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"
#include "PhysicalObject.h"

#define PLAYER_MESH "models/body_of_bike.obj"
#define WHEEL_MESH "models/note.obj"

enum DuckDir { NONE, LEFT, RIGHT };

class Player : public GameObject {
 public:
  // Represents size of gap between grounded platform
  static const float PLATFORM_SPACING;
  // TODO(jarhar): consider removing initial position
  static const glm::vec3 INITIAL_POSITION;

  Player(glm::vec3 position = INITIAL_POSITION,
         glm::vec3 rotation_axis = glm::vec3(1, 0, 0),
         float rotation_angle = 0,
         glm::vec3 scale = glm::vec3(0.8, 0.8, 0.8));
  virtual ~Player();

  ObjectType GetType() override;
  SecondaryType GetSecondaryType() override;

  void Reset();

  float GetYVelocity();
  float GetZVelocity();
  bool GetDoubleJump();
  std::shared_ptr<GameObject> GetGround();  // null if no ground
  int GetScore();

  void SetYVelocity(float y_velocity);
  void MoveDownZ();
  void MoveUpZ();
  void SetZVelocity(float z_velocity);
  void SetDoubleJump(bool can_double_jump);
  void SetDucking(DuckDir ducking);
  void SetGround(std::shared_ptr<GameObject> ground);
  void RemoveGround();
  void SetScore(int score);

 private:
  std::shared_ptr<GameObject> ground;
  bool can_double_jump;
  float y_velocity;
  float z_velocity;
  int score;
};

#endif
