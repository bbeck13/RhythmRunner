#include "Monster.h"

namespace gameobject {
std::shared_ptr<Shape> Monster::shape = std::make_shared<Shape>();
bool Monster::isInitialized = false;

Monster::Monster()
    : MovingObject(std::vector<glm::vec3>(), glm::vec3(0, 0, 0), 0.0f),
      Obstacle(Monster::shape) {}

Monster::Monster(glm::vec3 position)
    : MovingObject(Monster::default_path(position, 8, 3), position, 0.03f),
      Obstacle(Monster::shape) {
  this->position = position;
  this->scale = glm::vec3(1, 1, 1);
  this->model = shape;
}
Monster::Monster(glm::vec3 position,
                 glm::vec3 scale,
                 glm::vec3 rotation_axis,
                 float rotation_angle,
                 glm::vec3 velocity,
                 float disantceX,
                 float distanceZ)
    : MovingObject(Monster::default_path(position, disantceX, distanceZ),
                   position,
                   velocity),
      Obstacle(Monster::shape) {
  this->position = position;
  this->scale = scale;
  this->rotation_angle = rotation_angle;
  this->rotation_axis = rotation_axis;
}

Monster::Monster(glm::vec3 position,
                 glm::vec3 scale,
                 glm::vec3 rotation_axis,
                 float rotation_angle,
                 glm::vec3 velocity,
                 std::vector<glm::vec3> path)
    : MovingObject(path, position, velocity), Obstacle(Monster::shape) {
  this->position = position;
  this->scale = scale;
  this->rotation_angle = rotation_angle;
  this->rotation_axis = rotation_axis;
}

Monster::~Monster() {}

std::shared_ptr<Shape> Monster::GetModel() {
  if (!Monster::isInitialized) {
    Monster::shape->loadMesh(std::string(ASSET_DIR) + "/" +
                             std::string(MONSTER_MASH));
    Monster::shape->init();
    Monster::isInitialized = true;
  }
  return Monster::shape;
}

ObjectType Monster::GetType() {
  return ObjectType::OBSTACLE;
}

SecondaryType Monster::GetSecondaryType() {
  return SecondaryType::MONSTER;
}

std::vector<glm::vec3> Monster::default_path(glm::vec3 position,
                                             float schadenfreude,
                                             float kummerspeck) {
  std::vector<glm::vec3> path = std::vector<glm::vec3>();
  path.push_back(position + glm::vec3(schadenfreude, 0, kummerspeck));
  path.push_back(position + glm::vec3(schadenfreude * 2, 0, -kummerspeck));
  path.push_back(position);

  return path;
}
}
