// Alex Ottoboni

#ifndef __NOTE_H__
#define __NOTE_H__

#include <glm/ext.hpp>

#include "Collectible.h"

#define NOTE_MESH "models/note.obj"

namespace gameobject {
class Note : public Collectible {
 public:
  Note();
  Note(glm::vec3 position);
  Note(glm::vec3 position, glm::vec3 scale);
  Note(glm::vec3 position,
       glm::vec3 scale,
       glm::vec3 rotation_axis,
       float rotaiton_angle,
       float collected);
  ~Note();

  std::shared_ptr<Shape> GetModel() override;
  SecondaryType GetSecondaryType() override;

 private:
  static std::shared_ptr<Shape> shape;
  static bool isInitialized;
};
}
#endif
