// Joseph Arhar

#include "game_state/Player.h"

const float Player::GRAVITY = 0.2f;

Player::Player(std::shared_ptr<Shape> model)
    : GameObject(model), vertical_velocity(0) {}

Player::~Player() {}

float Player::GetVerticalVelocity() {
  return vertical_velocity;
}

void Player::SetVerticalVelocity(float vertical_velocity) {
  this->vertical_velocity = vertical_velocity;
}
