// Joseph Arhar

#include "game_state/Player.h"

const float Player::GRAVITY = 0.2f;

bool Player::isInitialized = false;

std::shared_ptr<Shape> Player::shape = std::make_shared<Shape>();

Player::Player(glm::vec3 position)
    : GameObject(Player::shape), vertical_velocity(0) {
  this->position = position;
  this->model = shape;
}

Player::~Player() {}

float Player::GetVerticalVelocity() {
  return vertical_velocity;
}

void Player::SetVerticalVelocity(float vertical_velocity) {
  this->vertical_velocity = vertical_velocity;
}

std::shared_ptr<Shape> Player::GetShape() {
  if (!isInitialized) {
    shape->loadMesh(PLAYER_MESH);
    shape->resize();
    shape->init();
    isInitialized = true;
  }
  return shape;
}
