// Joseph Arhar

#include "Player.h"

#include "TimingConstants.h"

// static
const float Player::PLATFORM_SPACING = 0.01f;
const glm::vec3 Player::INITIAL_POSITION(-5 - (DELTA_X_PER_SECOND * PREGAME_SECONDS), 4, -5);

// static
bool Player::isInitialized = false;
std::shared_ptr<Shape> Player::shape = std::make_shared<Shape>();

Player::Player() : GameObject(Player::shape), y_velocity(0) {
  SetPosition(Player::INITIAL_POSITION);
  this->model = shape;
  this->scale = glm::vec3(0.8, 0.8, 0.8);
  this->score = 0;
  this->y_velocity = 0;
  this->z_velocity = 0;
  this->can_double_jump = false;
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

std::shared_ptr<Shape> Player::GetModel() {
  if (!Player::isInitialized) {
    Player::shape->loadMesh(ASSET_DIR "/" PLAYER_MESH);
    Player::shape->init();
    Player::isInitialized = true;
  }
  return Player::shape;
}

void Player::SetYVelocity(float y_velocity) {
  this->y_velocity = y_velocity;
}

void Player::SetDoubleJump(bool can_double_jump) {
  this->can_double_jump = can_double_jump;
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
