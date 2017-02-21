// Alex Ottoboni

#ifndef __SKY_H__
#define __SKY_H__
#include "GameObject.h"
#include "Shape.h"
#include <glm/ext.hpp>

#define SKY_MESH "models/sphere.obj"

class Sky : public GameObject {
  public:
    Sky();
    Sky(glm::vec3 position, glm::vec3 scale);
    ~Sky();

    std::shared_ptr<Shape> GetModel() override;
    ObjectType GetType() override;
    SecondaryType GetSecondaryType() override;

  private:
    static std::shared_ptr<Shape> shape;
    static bool isInitialized;
};
#endif
