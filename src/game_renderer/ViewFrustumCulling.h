// Alex Ottoboni
#ifndef __VIEW_FRUST_CULLING__
#define __VIEW_FRUST_CULLING__

#define LEFT 0
#define RIGHT 1
#define BOTTOM 2
#define TOP 3
#define NEAR 4
#define FAR 5

#include <glm/gtc/type_ptr.hpp>
#include "game_state/AxisAlignedBox.h"

namespace ViewFrustumCulling {
    glm::vec4 * GetViewFrustumPlanes(glm::mat4 P, glm::mat4 V);
    float DistToPlane(float A, float B, float C, float D, glm::vec3 point);
    bool IsCulled(AxisAlignedBox box, glm::vec4 * planes); 
}
#endif
