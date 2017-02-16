// Alex Ottoboni

#ifndef __WHEEL_H__
#define __WHEEL_H__

#include <glm/ext.hpp>

#include "Collectible.h"

#define NOTE_MESH "models/wheels.obj"

class Wheels : public Collectible {
  public:
    Wheels(glm::vec3 position);
    Wheels(glm::vec3 position, glm::vec3 scale);
    ~Wheels();

    std::shared_ptr<Shape> GetModel() override;
    SecondaryType GetSecondaryType() override;

  private:
    static std::shared_ptr<Shape> shape;
    static bool isInitialized;
};
#endif
