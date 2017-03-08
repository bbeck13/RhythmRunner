// Joseph Arhar

#include "Player.h"

#include <cmath>
#include <iostream>

#include "TimingConstants.h"

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
      animation(Animation::JUMPING),
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

void Player::SetAnimation(Animation animation) {
  this->animation = animation;
}

void Player::SetAnimationStartTick(uint64_t animation_start_tick) {
  this->animation_start_tick = animation_start_tick;
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

Player::Animation Player::GetAnimation() {
  return animation;
}

std::shared_ptr<PhysicalObject> Player::GetRearWheel() {
  return rear_wheel;
}

std::shared_ptr<PhysicalObject> Player::GetFrontWheel() {
  return front_wheel;
}

void Player::MoveDownZ() {
  SetPosition(GetPosition() - glm::vec3(0, 0, PLAYER_DELTA_Z_PER_TICK));
}

void Player::MoveUpZ() {
  SetPosition(GetPosition() + glm::vec3(0, 0, PLAYER_DELTA_Z_PER_TICK));
}
