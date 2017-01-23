#include "HitBox.h"

HitBox::HitBox(float min_x, float max_x, float min_y, float max_y,
         float min_z, float max_z) {
   this->min_x = min_x;
   this->min_y = min_y;
   this->min_z = min_z;
   this->max_x = max_x;
   this->max_y = max_y;
   this->max_z = max_z;
}

HitBox::HitBox(float radius, vec3 center) {
   this->min_x = center.x + radius;
   this->min_y = center.y + radius;
   this->min_z = center.z + radius;
   this->max_x = center.x - radius;
   this->max_y = center.y - radius;
   this->max_z = center.z - radius;
}
