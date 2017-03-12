// Alex Ottoboni

#ifndef __MOONROCK_H__
#define __MOONROCK_H__

#include "GameObject.h"

#include <glm/ext.hpp>

#include "Texture.h"
#include "Program.h"

#define ROCK_MESH "models/rock.obj"

namespace gameobject {
class MoonRock : public GameObject {
 public:
  MoonRock(glm::vec3 position = glm::vec3(0, 0, 0),
           glm::vec3 scale = glm::vec3(1, 1, 1),
           float rotation_angle = 0,
           glm::vec3 rotation_axis = glm::vec3(0, 1, 0));
  virtual ~MoonRock();

  ObjectType GetType() override;
  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Program> moon_rock_program;
  static std::shared_ptr<Texture> moon_rock_texture;
};
}
#endif
