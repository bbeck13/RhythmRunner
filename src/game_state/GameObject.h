// Joseph Arhar

#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "PhysicalObject.h"

enum ObjectType { OBSTACLE, COLLECTIBLE, PLAYER, SCENERY };
enum SecondaryType {
  NOTE,
  PLATFORM,
  BIKE,
  MOVING_PLATFORM,
  DROPPING_PLATFORM,
  SKY,
  MOONROCK,
  PLAINROCK,
  MONSTER
};

// GameObjects are functional game entities which have one or more
// drawable, "physical" 3D entities in a tree
class GameObject : public PhysicalObject {
 public:
  GameObject(const std::string& shape_path,
             glm::vec3 position = glm::vec3(),
             glm::vec3 rotation_axis = glm::vec3(1, 0, 0),
             float rotation_angle = 0,
             glm::vec3 scale = glm::vec3(1, 1, 1));
  virtual ~GameObject();

  virtual ObjectType GetType() = 0;
  virtual SecondaryType GetSecondaryType() = 0;

  static bool Moves(SecondaryType type);
};

#endif
