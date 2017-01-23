#ifndef __HIT_BOX_H__
#define __HIT_BOX_H__

// value_ptr for glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class HitBox {
public:
   HitBox(float min_x, float max_x, float min_y, float max_y,
         float min_z, float max_z);
   HitBox(float radius, vec3 center);
private:
   float min_x, max_x;
   float min_y, max_y;
   float min_z, max_z;
};

#endif /*__HIT_BOX_H__*/
