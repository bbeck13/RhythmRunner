// Joseph Arhar

#include "game_state/Model.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Model::Model(const std::string& filename) : filename(filename) {
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string error_string;

  // TODO(jarhar): consider moving file access out of constructor
  if (!tinyobj::LoadObj(shapes, materials, error_string, filename.c_str())) {
    std::cerr << __FUNCTION__ << " tinyobj::LoadObj returned error: \""
              << error_string << "\"" << std::endl;
  } else {
    positions = shapes[0].mesh.positions;
    normals = shapes[0].mesh.normals;
    texture_coordinates = shapes[0].mesh.texcoords;
    elements = shapes[0].mesh.indices;
    Normalize();
  }
}

Model::~Model() {}

const std::string& Model::GetFilename() {
  return filename;
}

std::vector<unsigned>& Model::GetElements() {
  return elements;
}

std::vector<float>& Model::GetPositions() {
  return positions;
}

std::vector<float>& Model::GetNormals() {
  return normals;
}

std::vector<float>& Model::GetTextureCoordinates() {
  return texture_coordinates;
}

void Model::Normalize() {
  float minX, minY, minZ;
  float maxX, maxY, maxZ;
  float scaleX, scaleY, scaleZ;
  float shiftX, shiftY, shiftZ;
  float epsilon = 0.001;

  minX = minY = minZ = 1.1754E+38F;
  maxX = maxY = maxZ = -1.1754E+38F;

  // Go through all vertices to determine min and max of each dimension
  for (size_t v = 0; v < positions.size() / 3; v++) {
    if (positions[3 * v + 0] < minX)
      minX = positions[3 * v + 0];
    if (positions[3 * v + 0] > maxX)
      maxX = positions[3 * v + 0];

    if (positions[3 * v + 1] < minY)
      minY = positions[3 * v + 1];
    if (positions[3 * v + 1] > maxY)
      maxY = positions[3 * v + 1];

    if (positions[3 * v + 2] < minZ)
      minZ = positions[3 * v + 2];
    if (positions[3 * v + 2] > maxZ)
      maxZ = positions[3 * v + 2];
  }

  // From min and max compute necessary scale and shift for each dimension
  float maxExtent, xExtent, yExtent, zExtent;
  xExtent = maxX - minX;
  yExtent = maxY - minY;
  zExtent = maxZ - minZ;
  if (xExtent >= yExtent && xExtent >= zExtent) {
    maxExtent = xExtent;
  }
  if (yExtent >= xExtent && yExtent >= zExtent) {
    maxExtent = yExtent;
  }
  if (zExtent >= xExtent && zExtent >= yExtent) {
    maxExtent = zExtent;
  }
  scaleX = 2.0 / maxExtent;
  shiftX = minX + (xExtent / 2.0);
  scaleY = 2.0 / maxExtent;
  shiftY = minY + (yExtent / 2.0);
  scaleZ = 2.0 / maxExtent;
  shiftZ = minZ + (zExtent) / 2.0;

  // Go through all verticies shift and scale them
  for (size_t v = 0; v < positions.size() / 3; v++) {
    positions[3 * v + 0] = (positions[3 * v + 0] - shiftX) * scaleX;
    assert(positions[3 * v + 0] >= -1.0 - epsilon);
    assert(positions[3 * v + 0] <= 1.0 + epsilon);
    positions[3 * v + 1] = (positions[3 * v + 1] - shiftY) * scaleY;
    assert(positions[3 * v + 1] >= -1.0 - epsilon);
    assert(positions[3 * v + 1] <= 1.0 + epsilon);
    positions[3 * v + 2] = (positions[3 * v + 2] - shiftZ) * scaleZ;
    assert(positions[3 * v + 2] >= -1.0 - epsilon);
    assert(positions[3 * v + 2] <= 1.0 + epsilon);
  }
}
