// Alex Ottoboni

#ifndef __NOTE_H__
#define __NOTE_H__

#include <glm/ext.hpp>

#include "Collectible.h"

#define NOTE_MESH "models/note.obj"

class Note : public Collectible {
  public:
    Note(glm::vec3 position);
    Note(glm::vec3 position, glm::vec3 scale);
    ~Note();

    std::shared_ptr<Shape> GetModel() override;
    SecondaryType GetSecondaryType() override;

  private:
    static std::shared_ptr<Shape> shape;
    static bool isInitialized;
};
#endif
