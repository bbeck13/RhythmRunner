// Alex Ottoboni

#ifndef __SKY_H__
#define __SKY_H__

#include <glm/ext.hpp>

#include "Collectible.h"

#define SKY_MESH "models/sphere.obj"

class Sky : public Collectible {
  public:
    Sky();
    Sky(glm::vec3 position, glm::vec3 scale);
    ~Sky();

    std::shared_ptr<Shape> GetModel() override;
    SecondaryType GetSecondaryType() override;

  private:
    static std::shared_ptr<Shape> shape;
    static bool isInitialized;
};
#endif
