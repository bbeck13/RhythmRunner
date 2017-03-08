// Joseph Arhar

#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"
#include "PhysicalObject.h"

#define PLAYER_MESH "models/body_of_bike.obj"
#define WHEEL_MESH "models/note.obj"

#define WHEEL_SCALE 0.4
#define WHEEL_ROTATION_PER_SECOND 12.0
#define WHEEL_ROTATION_PER_TICK (WHEEL_ROTATION_PER_SECOND * SECONDS_PER_TICK)

// TODO(jarhar): convert this to an animation
enum DuckDir { NONE, LEFT, RIGHT };

class Player : public GameObject {
 public:
  // Player animation stuff
  static const int ANIMATION_ENDGAME_BIT = 1 << 5;
  static const int ANIMATION_WHEELSPIN_BIT = 1 << 6;
  static const int ANIMATION_WHEELSPIN_SLOW_BIT = 1 << 7;
  enum Animation {
    GROUNDED = 1 | ANIMATION_WHEELSPIN_BIT,
    JUMPSQUAT = 2 | ANIMATION_WHEELSPIN_SLOW_BIT,
    JUMPING = 3 | ANIMATION_WHEELSPIN_SLOW_BIT,
    SUCCESS = 4 | ANIMATION_ENDGAME_BIT,
    FAILURE = 5 | ANIMATION_ENDGAME_BIT | ANIMATION_WHEELSPIN_BIT,
    LANDING = 6 | ANIMATION_WHEELSPIN_SLOW_BIT
  };
  static std::string AnimationToString(Animation animation) {
    switch (animation) {
      case GROUNDED:
        return "GROUNDED";
      case JUMPSQUAT:
        return "JUMPSQUAT";
      case JUMPING:
        return "JUMPING";
      case SUCCESS:
        return "SUCCESS";
      case FAILURE:
        return "FAILURE";
      case LANDING:
        return "LANDING";
      default:
        return "unknown animation";
    }
  }

  // Represents size of gap between grounded platform
  static const float PLATFORM_SPACING;
  // TODO(jarhar): consider removing initial position
  static const glm::vec3 INITIAL_POSITION;

  Player(glm::vec3 position = INITIAL_POSITION,
         glm::vec3 rotation_axis = glm::vec3(0, 0, 1),
         float rotation_angle = 0,
         glm::vec3 scale = glm::vec3(0.8, 0.8, 0.8));
  virtual ~Player();

  ObjectType GetType() override;
  SecondaryType GetSecondaryType() override;

  void Reset();

  float GetYVelocity();
  float GetZVelocity();
  bool GetDoubleJump();
  DuckDir GetDucking();
  std::shared_ptr<GameObject> GetGround();  // null if no ground
  int GetScore();
  Animation GetAnimation();
  std::shared_ptr<PhysicalObject> GetRearWheel();
  std::shared_ptr<PhysicalObject> GetFrontWheel();

  void SetYVelocity(float y_velocity);
  void MoveDownZ();
  void MoveUpZ();
  void SetZVelocity(float z_velocity);
  void SetDoubleJump(bool can_double_jump);
  void SetDucking(DuckDir ducking);
  void SetGround(std::shared_ptr<GameObject> ground);
  void RemoveGround();
  void SetScore(int score);
  void SetAnimation(Animation animation);
  void SetAnimationStartTick(uint64_t animation_start_tick);

 private:
  std::shared_ptr<GameObject> ground;
  bool can_double_jump;
  float y_velocity;
  float z_velocity;
  int score;

  Animation animation;
  uint64_t animation_start_tick;
  float wheel_rotation_speed;
  std::shared_ptr<PhysicalObject> rear_wheel;
  std::shared_ptr<PhysicalObject> front_wheel;
};

#endif
