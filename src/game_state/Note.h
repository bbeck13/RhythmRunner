// Alex Ottoboni

#ifndef __NOTE_H__
#define __NOTE_H__

#include <glm/ext.hpp>

#include "GameObject.h"

#define NOTE_MESH "models/sphere.obj"

class Note : public GameObject {
  public:
    Note(glm::vec3 position);
    Note(glm::vec3 position, glm::vec3 scale);
    ~Note();

    void SetCollected();
    void SetUncollected();
    bool GetCollected();
    std::shared_ptr<Shape> GetModel() override;

  private:
    static std::shared_ptr<Shape> shape;
    static bool isInitialized;
    bool isCollected;
};
#endif
