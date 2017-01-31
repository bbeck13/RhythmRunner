// Joseph Arhar

#include "game_state/Player.h"

// static
const float Player::GRAVITY = 0.002f;
const float Player::JUMP_VELOCITY = Player::GRAVITY * 20.0f;
const float Player::PLATFORM_SPACING = 0.01f;
bool Player::isInitialized = false;

// static
std::shared_ptr<Shape> Player::shape = std::make_shared<Shape>();

Player::Player(glm::vec3 position)
    : GameObject(Player::shape), vertical_velocity(0), spacebar_down(false) {
  SetPosition(position);
  this->model = shape;
  this->scale = glm::vec3(0.3, 0.3, 0.3);
}

Player::~Player() {}

float Player::GetVerticalVelocity() {
  return vertical_velocity;
}

std::shared_ptr<GameObject> Player::GetGround() {
  return ground;
}

bool Player::GetSpacebarDown() {
  return spacebar_down;
}

std::shared_ptr<Shape> Player::GetModel() {
  if (!Player::isInitialized) {
    Player::shape->loadMesh(PLAYER_MESH);
    Player::shape->init();
    Player::isInitialized = true;
  }
  return Player::shape;
}

void Player::SetVerticalVelocity(float vertical_velocity) {
  this->vertical_velocity = vertical_velocity;
}

void Player::SetSpacebarDown(bool spacebar_down) {
  this->spacebar_down = spacebar_down;
}

void Player::SetGround(GameObject ground) {
  this->ground = std::make_shared<GameObject>(ground);
}

void Player::RemoveGround() {
  ground.reset();
}
