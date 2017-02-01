// Joseph Arhar

#include <string>
#include "Player.h"

const float Player::GRAVITY = 0.002f;

bool Player::isInitialized = false;

std::shared_ptr<Shape> Player::shape = std::make_shared<Shape>();

Player::Player(glm::vec3 position)
    : GameObject(Player::shape), vertical_velocity(0) {
  this->position = position;
  this->model = shape;
  this->scale = glm::vec3(-0.3, 0.3, 0.3);
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
    shape->loadMesh(std::string(ASSET_DIR) + "/" + std::string(PLAYER_MESH));
    shape->init();
    isInitialized = true;
  }
  return shape;
}
