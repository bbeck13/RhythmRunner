#include "ViewFrustumCulling.h"

namespace ViewFrustumCulling {

   // Code from CPE-476 lab
   glm::vec4 * GetViewFrustumPlanes(glm::mat4 P, glm::mat4 V) {
     glm::vec4 Left, Right, Bottom, Top, Near, Far;
     glm::vec4 *planes = (glm::vec4 *)malloc(sizeof(glm::vec4) * 6);
     glm::vec3 normal;
     float normal_length;
    
     glm::mat4 comp = P * V;
     
     Left.x = comp[0][3] + comp[0][0];
     Left.y = comp[1][3] + comp[1][0];
     Left.z = comp[2][3] + comp[2][0];
     Left.w = comp[3][3] + comp[3][0];
     normal = glm::vec3(Left.x, Left.y, Left.z);
     normal_length = glm::length(normal);
     planes[0] = Left/normal_length;
     Left = Left/normal_length;

     Right.x = comp[0][3] - comp[0][0];
     Right.y = comp[1][3] - comp[1][0];
     Right.z = comp[2][3] - comp[2][0];
     Right.w = comp[3][3] - comp[3][0];
     normal = glm::vec3(Right.x, Right.y, Right.z);
     normal_length = glm::length(normal);
     planes[1] = Right/normal_length;
     Right = Right/normal_length;

     Bottom.x = comp[0][3] + comp[0][1];
     Bottom.y = comp[1][3] + comp[1][1];
     Bottom.z = comp[2][3] + comp[2][1];
     Bottom.w = comp[3][3] + comp[3][1];
     normal = glm::vec3(Bottom.x, Bottom.y, Bottom.z);
     normal_length = glm::length(normal);
     planes[2] = Bottom/normal_length;
     Bottom = Bottom/normal_length;

     Top.x = comp[0][3] - comp[0][1];
     Top.y = comp[1][3] - comp[1][1];
     Top.z = comp[2][3] - comp[2][1];
     Top.w = comp[3][3] - comp[3][1];
     normal = glm::vec3(Top.x, Top.y, Top.z);
     normal_length = glm::length(normal);
     planes[3] = Top/normal_length;
     Top = Top/normal_length;

     Near.x = comp[0][3] + comp[0][2];
     Near.y = comp[1][3] + comp[1][2];
     Near.z = comp[2][3] + comp[2][2];
     Near.w = comp[3][3] + comp[3][2];
     normal = glm::vec3(Near.x, Near.y, Near.z);
     normal_length = glm::length(normal);
     planes[4] = Near/normal_length;
     Near = Near/normal_length;

     Far.x = comp[0][3] - comp[0][2];
     Far.y = comp[1][3] - comp[1][2];
     Far.z = comp[2][3] - comp[2][2];
     Far.w = comp[3][3] - comp[3][2];
     normal = glm::vec3(Far.x, Far.y, Far.z);
     normal_length = glm::length(normal);
     planes[5] = Far/normal_length;
     Far = Far/normal_length;

     return planes;
   }

   float DistToPlane(float A, float B, float C, float D, glm::vec3 point) {
     return (A * point.x + B * point.y + C * point.z + D);
   }

   bool IsCulled(AxisAlignedBox box, glm::vec4 * planes) {
     float dist;

     // If the max point is on the left side of the plane, cull
     dist = DistToPlane(planes[LEFT].x, planes[LEFT].y, planes[LEFT].z, planes[LEFT].w, box.GetMax());
     if (dist <= 0) {
       return true;
     }

     // If the min point is on the right side of the plane, cull
     dist = DistToPlane(planes[RIGHT].x, planes[RIGHT].y, planes[RIGHT].z, planes[RIGHT].w, box.GetMin());
     if (dist <= 0) {
       return true;
     }

     // If the min point is above the plane, cull
     dist = DistToPlane(planes[TOP].x, planes[TOP].y, planes[TOP].z, planes[TOP].w, box.GetMin());
     if (dist <= 0) {
       return true;
     }

     // If the max point is below the plane, cull
     dist = DistToPlane(planes[BOTTOM].x, planes[BOTTOM].y, planes[BOTTOM].z, planes[BOTTOM].w, box.GetMax());
     if (dist <= 0) {
       return true;
     }

     // If the max point is in front of the plane, cull
     dist = DistToPlane(planes[NEAR].x, planes[NEAR].y, planes[NEAR].z, planes[NEAR].w, box.GetMax());
     if (dist <= 0) {
       return true;
     }

     // If the min point is in behind the plane, cull
     dist = DistToPlane(planes[FAR].x, planes[FAR].y, planes[FAR].z, planes[FAR].w, box.GetMax());
     if (dist <= 0) {
       return true;
     }

     return false;
   } 
}
