// Joseph Arhar

#include <string>
#include "Player.h"

// static
const float Player::PLATFORM_SPACING = 0.01f;
const glm::vec3 Player::INITIAL_POSITION(-5.5, 3, -5);

// static
bool Player::isInitialized = false;
std::shared_ptr<Shape> Player::shape = std::make_shared<Shape>();

Player::Player()
    : GameObject(Player::shape), vertical_velocity(0), spacebar_down(false) {
  SetPosition(Player::INITIAL_POSITION);
  this->model = shape;
  this->scale = glm::vec3(0.8, 0.8, 0.8);
  this->score = 0;
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
    Player::shape->loadMesh(std::string(ASSET_DIR) + "/" +
                            std::string(PLAYER_MESH));
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
