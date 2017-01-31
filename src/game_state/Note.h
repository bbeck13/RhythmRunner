// Alex Ottoboni

#ifndef NOTE_H
#define NOTE_H

#include "GameObject.h"

#define NOTE "../assets/shaders/note.json"
#define NOTE_MESH "../assets/models/note.obj"

class Note : public GameObject {
  public:
    Note(glm::vec3 position);
    Note(glm::vec3 position, glm::vec3 scale);
    ~Note();
    static std::shared_ptr<Shape> GetShape();
  private:
    static std::shared_ptr<Shape> shape;
    static bool isInitialized;
    static bool isCollected;
};
#endif
