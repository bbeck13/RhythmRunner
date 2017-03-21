#include "ViewFrustumCulling.h"

#include <iostream>
#include <ctime>

namespace ViewFrustumCulling {

// Code from CPE-476 lab
std::shared_ptr<std::vector<glm::vec4>> GetViewFrustumPlanes(glm::mat4 P,
                                                             glm::mat4 V) {
  glm::vec4 Left, Right, Bottom, Top, Near, Far;
  glm::vec3 normal;
  float normal_length;
  std::shared_ptr<std::vector<glm::vec4>> planes =
      std::make_shared<std::vector<glm::vec4>>();
  planes->reserve(6);

  glm::mat4 comp = P * V;

  Left.x = comp[0][3] + comp[0][0];
  Right.x = comp[0][3] - comp[0][0];
  Bottom.x = comp[0][3] + comp[0][1];
  Top.x = comp[0][3] - comp[0][1];
  Near.x = comp[0][3] + comp[0][2];
  Far.x = comp[0][3] - comp[0][2];

  Left.y = comp[1][3] + comp[1][0];
  Right.y = comp[1][3] - comp[1][0];
  Bottom.y = comp[1][3] + comp[1][1];
  Top.y = comp[1][3] - comp[1][1];
  Far.y = comp[1][3] - comp[1][2];
  Near.y = comp[1][3] + comp[1][2];

  Left.z = comp[2][3] + comp[2][0];
  Right.z = comp[2][3] - comp[2][0];
  Bottom.z = comp[2][3] + comp[2][1];
  Top.z = comp[2][3] - comp[2][1];
  Near.z = comp[2][3] + comp[2][2];
  Far.z = comp[2][3] - comp[2][2];

  Left.w = comp[3][3] + comp[3][0];
  Right.w = comp[3][3] - comp[3][0];
  Bottom.w = comp[3][3] + comp[3][1];
  Top.w = comp[3][3] - comp[3][1];
  Near.w = comp[3][3] + comp[3][2];
  Far.w = comp[3][3] - comp[3][2];

  normal_length = glm::length(glm::vec3(Left.x, Left.y, Left.z));
  Left = Left / normal_length;
  normal_length = glm::length(glm::vec3(Right.x, Right.y, Right.z));
  Right = Right / normal_length;
  normal_length = glm::length(glm::vec3(Bottom.x, Bottom.y, Bottom.z));
  Bottom = Bottom / normal_length;
  normal_length = glm::length(glm::vec3(Top.x, Top.y, Top.z));
  Top = Top / normal_length;
  normal_length = glm::length(glm::vec3(Near.x, Near.y, Near.z));
  Near = Near / normal_length;
  normal_length = glm::length(glm::vec3(Far.x, Far.y, Far.z));
  Far = Far / normal_length;

  planes->push_back(Left);
  planes->push_back(Right);
  planes->push_back(Bottom);
  planes->push_back(Top);
  planes->push_back(Near);
  planes->push_back(Far);

  return planes;
}

float DistToPlane(float A, float B, float C, float D, glm::vec3 point) {
  return (A * point.x + B * point.y + C * point.z + D);
}

bool IsCulled(AxisAlignedBox box,
              std::shared_ptr<std::vector<glm::vec4>> planes) {
  float dist;

  std::vector<glm::vec3> box_vertices;
  // Right edge of box
  box_vertices.push_back(box.GetMax());
  box_vertices.push_back(vec3(box.GetMax().x, box.GetMax().y, box.GetMin().z));
  box_vertices.push_back(vec3(box.GetMax().x, box.GetMin().y, box.GetMin().z));
  box_vertices.push_back(vec3(box.GetMax().x, box.GetMin().y, box.GetMax().z));

  // Left edge of box
  box_vertices.push_back(box.GetMin());
  box_vertices.push_back(vec3(box.GetMin().x, box.GetMax().y, box.GetMin().z));
  box_vertices.push_back(vec3(box.GetMin().x, box.GetMin().y, box.GetMin().z));
  box_vertices.push_back(vec3(box.GetMin().x, box.GetMin().y, box.GetMax().z));

  int cull_count = 0;
  for (glm::vec3 box_vert : box_vertices) {
    dist =
        DistToPlane(planes->at(VFC_LEFT).x, planes->at(VFC_LEFT).y,
                    planes->at(VFC_LEFT).z, planes->at(VFC_LEFT).w, box_vert);
    if (dist <= 0) {
      cull_count++;
    }
  }
  if (cull_count == 8) {
    return true;
  }

  cull_count = 0;
  for (glm::vec3 box_vert : box_vertices) {
    dist =
        DistToPlane(planes->at(VFC_RIGHT).x, planes->at(VFC_RIGHT).y,
                    planes->at(VFC_RIGHT).z, planes->at(VFC_RIGHT).w, box_vert);
    if (dist <= 0) {
      cull_count++;
    }
  }
  if (cull_count == 8) {
    return true;
  }

  cull_count = 0;
  for (glm::vec3 box_vert : box_vertices) {
    dist = DistToPlane(planes->at(VFC_TOP).x, planes->at(VFC_TOP).y,
                       planes->at(VFC_TOP).z, planes->at(VFC_TOP).w, box_vert);
    if (dist <= 0) {
      cull_count++;
    }
  }
  if (cull_count == 8) {
    return true;
  }

  cull_count = 0;
  for (glm::vec3 box_vert : box_vertices) {
    dist = DistToPlane(planes->at(VFC_BOTTOM).x, planes->at(VFC_BOTTOM).y,
                       planes->at(VFC_BOTTOM).z, planes->at(VFC_BOTTOM).w,
                       box_vert);
    if (dist <= 0) {
      cull_count++;
    }
  }
  if (cull_count == 8) {
    return true;
  }

  cull_count = 0;
  for (glm::vec3 box_vert : box_vertices) {
    dist =
        DistToPlane(planes->at(VFC_NEAR).x, planes->at(VFC_NEAR).y,
                    planes->at(VFC_NEAR).z, planes->at(VFC_NEAR).w, box_vert);
    if (dist <= 0) {
      cull_count++;
    }
  }
  if (cull_count == 8) {
    return true;
  }

  cull_count = 0;
  for (glm::vec3 box_vert : box_vertices) {
    dist = DistToPlane(planes->at(VFC_FAR).x, planes->at(VFC_FAR).y,
                       planes->at(VFC_FAR).z, planes->at(VFC_FAR).w, box_vert);
    if (dist <= 0) {
      cull_count++;
    }
  }
  if (cull_count == 8) {
    return true;
  }

  return false;
}
}
