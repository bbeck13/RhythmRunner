// bnbeck

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <glm/ext.hpp>

#include "GameObject.h"
#include "Obstacle.h"

#define PLATFORM_MESH "models/platform.obj"

class Platform : public Obstacle {
 public:
   Platform(glm::vec3 position);
   Platform(glm::vec3 position, glm::vec3 scale);
   ~Platform();

   std::shared_ptr<Shape> GetModel() override;
   SecondaryType GetSecondaryType() override;

 private:
   static std::shared_ptr<Shape> platform;
   static bool isInitialized;
};
#endif
