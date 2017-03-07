// Joseph Arhar

#include "Player.h"

#include "TimingConstants.h"

#define WHEEL_SCALE 0.4
#define WHEEL_ROTATION_PER_SECOND 12.0
#define WHEEL_ROTATION_PER_TICK (WHEEL_ROTATION_PER_SECOND * SECONDS_PER_TICK)

// static
const float Player::PLATFORM_SPACING = 0.01f;
const glm::vec3 Player::INITIAL_POSITION(-2 - (DELTA_X_PER_SECOND *
                                               PREGAME_SECONDS),
                                         7,
                                         -5);

Player::Player(glm::vec3 position,
               glm::vec3 rotation_axis,
               float rotation_angle,
               glm::vec3 scale)
    : GameObject(PLAYER_MESH, position, rotation_axis, rotation_angle, scale),
      score(0),
      y_velocity(0),
      z_velocity(0),
      can_double_jump(false),
      current_animation(Animation::JUMPING),
      wheel_rotation_speed(0) {
  rear_wheel = std::make_shared<PhysicalObject>(
      WHEEL_MESH, glm::vec3(-1.2, -0.3, 0), glm::vec3(0, 0, -1), 0,
      glm::vec3(WHEEL_SCALE, WHEEL_SCALE, WHEEL_SCALE));
  front_wheel = std::make_shared<PhysicalObject>(
      WHEEL_MESH, glm::vec3(0.9, -0.3, 0), glm::vec3(0, 0, -1), 0,
      glm::vec3(WHEEL_SCALE, WHEEL_SCALE, WHEEL_SCALE));

  AddSubObject(rear_wheel);
  AddSubObject(front_wheel);
}

Player::~Player() {}

float Player::GetYVelocity() {
  return y_velocity;
}

bool Player::GetDoubleJump() {
#ifdef DEBUG
  return true;
#endif
  return can_double_jump;
}

std::shared_ptr<GameObject> Player::GetGround() {
  return ground;
}

void Player::Reset() {
  SetPosition(Player::INITIAL_POSITION);
  score = 0;
  y_velocity = 0;
  z_velocity = 0;
  can_double_jump = false;
  RemoveGround();
}

void Player::SetYVelocity(float y_velocity) {
  this->y_velocity = y_velocity;
}

void Player::SetDoubleJump(bool can_double_jump) {
  this->can_double_jump = can_double_jump;
}

void Player::SetDucking(DuckDir ducking) {
  switch (ducking) {
    case NONE:
      SetRotationAngle(0.0f);
      break;
    case LEFT:
      SetRotationAngle(-1.05f);
      break;
    case RIGHT:
      SetRotationAngle(1.05f);
      break;
    default:
      SetRotationAngle(0.0f);
      break;
  }
}

DuckDir Player::GetDucking() {
  if (this->rotation_angle < 0) {
    return LEFT;
  } else if (this->rotation_angle > 0) {
    return RIGHT;
  } else {
    return NONE;
  }
  return NONE;
}

void Player::SetGround(std::shared_ptr<GameObject> ground) {
  this->ground = ground;
}

void Player::RemoveGround() {
  ground.reset();
}

void Player::SetScore(int score) {
  this->score = score;
}

int Player::GetScore() {
  return this->score;
}

ObjectType Player::GetType() {
  return ObjectType::PLAYER;
}

float Player::GetZVelocity() {
  return z_velocity;
}

void Player::SetZVelocity(float z_velocity) {
  this->z_velocity = z_velocity;
}

SecondaryType Player::GetSecondaryType() {
  return SecondaryType::BIKE;
}
void Player::MoveDownZ() {
  SetPosition(GetPosition() - glm::vec3(0, 0, PLAYER_DELTA_Z_PER_TICK));
}

void Player::MoveUpZ() {
  SetPosition(GetPosition() + glm::vec3(0, 0, PLAYER_DELTA_Z_PER_TICK));
}

void Player::SnapToGround() {
  if (!GetGround()) {
    return;
  }

  AxisAlignedBox bounding_box = GetBoundingBox();
  AxisAlignedBox ground_box = GetGround()->GetBoundingBox();

  SetPosition(
      glm::vec3(GetPosition().x,
                ground_box.GetMax().y + Player::PLATFORM_SPACING +
                    (bounding_box.GetMax().y - bounding_box.GetMin().y) / 2 +
                    (GetPosition().y - bounding_box.GetCenter().y),
                GetPosition().z));
}

void Player::ChangeAnimation(Animation new_animation, uint64_t current_tick) {
  animation_start_tick = current_tick - 1;  // TODO(jarhar): this is hacky
  current_animation = new_animation;
}

void Player::Animate(uint64_t current_tick) {
  // Rotate the wheels
  if (current_animation == Animation::GROUNDED) {
    wheel_rotation_speed = WHEEL_ROTATION_PER_TICK;
  } else if (current_animation == Animation::JUMPING) {
    wheel_rotation_speed *= 0.98;
  }
  rear_wheel->SetRotationAngle(rear_wheel->GetRotationAngle() +
                               wheel_rotation_speed);
  front_wheel->SetRotationAngle(front_wheel->GetRotationAngle() +
                                wheel_rotation_speed);

  // TODO(jarhar): set player rotation based on y velocity
}

void Player::Jump(uint64_t current_tick) {
  // TODO(jarhar): create a particle effect here?
  ChangeAnimation(Animation::JUMPING, current_tick);
  SetYVelocity(PLAYER_JUMP_VELOCITY);
  SetZVelocity(0);
  RemoveGround();
}
