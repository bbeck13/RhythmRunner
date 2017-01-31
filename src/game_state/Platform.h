// bnbeck

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <glm/ext.hpp>

#include "GameObject.h"

#define PLATFORM "../assets/shaders/platform.json"
#define PLATFORM_MESH "../assets/models/platform.obj"

class Platform : public GameObject {
 public:
   Platform(glm::vec3 position);
   Platform(glm::vec3 position, glm::vec3 scale);
   ~Platform();

   std::shared_ptr<Shape> GetModel() override;
 private:
   static std::shared_ptr<Shape> platform;
   static bool isInitialized;
};
#endif
