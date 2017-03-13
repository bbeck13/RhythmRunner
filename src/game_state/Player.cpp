// Joseph Arhar

#include "Player.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "TimingConstants.h"
#include "DroppingPlatform.h"
#include "MovingObject.h"
#include "GameRenderer.h"

#define MAX_DUCK_ANGLE 1.05f
#define DUCK_FINISH_SECONDS 0.2
#define DUCK_FINISH_TICKS (DUCK_FINISH_SECONDS * TICKS_PER_SECOND)

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

  program = GameRenderer::ProgramFromJSON(ASSET_DIR "/shaders/player.json");
  texture =
      GameRenderer::TextureFromJSON(ASSET_DIR "/textures/rainbowglass.json");

  AddSubObject(rear_wheel);
  AddSubObject(front_wheel);

  blocked_up_z = false;
  blocked_down_z = false;
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
  can_double_jump = blocked_up_z = blocked_down_z = false;
  RemoveGround();
}

void Player::SetYVelocity(float y_velocity) {
  this->y_velocity = y_velocity;
}

void Player::SetDoubleJump(bool can_double_jump) {
  this->can_double_jump = can_double_jump;
}

void Player::SetDucking(DuckDir duck_dir) {
  this->duck_dir = duck_dir;
  duck_start_tick = current_tick;
}

Player::DuckDir Player::GetDucking() {
  return duck_dir;
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

float Player::GetWheelRotationSpeed() {
  return wheel_rotation_speed;
}

void Player::SetWheelRotationSpeed(float wheel_rotation_speed) {
  this->wheel_rotation_speed = wheel_rotation_speed;
}

glm::mat4 Player::GetRotationMatrix() const {
  float duck_rotation;
  float duck_ratio = std::min(
      (current_tick - duck_start_tick) / (float)DUCK_FINISH_TICKS, 1.0f);
  switch (duck_dir) {
    case DuckDir::LEFT:
      duck_rotation = duck_ratio * -MAX_DUCK_ANGLE;
      break;
    case DuckDir::RIGHT:
      duck_rotation = duck_ratio * MAX_DUCK_ANGLE;
      break;
    case DuckDir::NONE:
      duck_rotation = 0.0f;
      break;
  }

  // rotate first for aerial rocking, then for ducking
  return glm::rotate(glm::mat4(1.0), rotation_angle, rotation_axis) *
         glm::rotate(glm::mat4(1.0), duck_rotation, glm::vec3(1, 0, 0));
}

float Player::GetGroundYVelocity() {
  if (!GetGround()) {
    return 0;
  }

  std::shared_ptr<gameobject::DroppingPlatform> dropping_platform;
  std::shared_ptr<MovingObject> moving_object;
  if ((dropping_platform =
           std::dynamic_pointer_cast<gameobject::DroppingPlatform>(
               GetGround()))) {
    return dropping_platform->GetYVelocity();
  } else if ((moving_object =
                  std::dynamic_pointer_cast<MovingObject>(GetGround()))) {
    return moving_object->GetMovementVector().y *
           moving_object->GetVelocity().y;
  }

  return 0;
}

void Player::SetCurrentTick(uint64_t current_tick) {
  this->current_tick = current_tick;
}

void Player::SetBlockedUpZ(bool blocked) {
  this->blocked_up_z = blocked;
}

void Player::SetBlockedDownZ(bool blocked) {
  this->blocked_down_z = blocked;
}

bool Player::GetBlockedUpZ() {
  return blocked_up_z;
}

bool Player::GetBlockedDownZ() {
  return blocked_down_z;
}
