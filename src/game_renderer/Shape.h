#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>

#include "tiny_obj_loader.h"

class Program;

class Shape {
 public:
  Shape();
  virtual ~Shape();

  static std::vector<tinyobj::shape_t> GetShapesFromObj(
      const std::string& filepath);

  void loadMesh(const std::string& meshName);
  void init();
  void draw(const std::shared_ptr<Program> prog) const;

  std::vector<float> GetPositions();

 private:
  void Normalize();
  void ComputeTex();

  std::vector<unsigned int> elements;
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> texcoords;
  unsigned elements_id;
  unsigned positions_id;
  unsigned normals_id;
  unsigned texcoords_id;
  unsigned vao_id;
};

#endif
