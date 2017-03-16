#include "Shape.h"

#include <iostream>

#include "GLSL.h"
#include "Program.h"
#include "math.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define EPSILON_SHAPE 0.001;
#include <cmath>

Shape::Shape(tinyobj::shape_t shape) : indices_id(0), positions_id(0), normals_id(0), texcoords_id(0), vao_id(0) {}

Shape::~Shape() {}

void Shape::loadMesh(const std::string& meshName) {
  // Load geometry
  // Some obj files contain material information.
  // We'll ignore them for this assignment.
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> objMaterials;
  std::string errStr;
  bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
  if (!rc) {
    std::cerr << errStr << std::endl;
  } else {
    positions = shapes[0].mesh.positions;
    normals = shapes[0].mesh.normals;
    texcoords = shapes[0].mesh.texcoords;
    indices = shapes[0].mesh.indices;
    Normalize();
  }
}

/* Note this is fairly dorky - */
void Shape::ComputeTex() {
  float u, v;

  for (size_t n = 0; n < normals.size() / 3; n++) {
    u = normals[n * 3 + 0] / 2.0 + 0.5;
    v = normals[n * 3 + 1] / 2.0 + 0.5;
    texcoords[n * 3 + 0] = u;
    texcoords[n * 3 + 1] = v;
  }
}

void Shape::Normalize() {
  float minX, minY, minZ;
  float maxX, maxY, maxZ;
  float scaleX, scaleY, scaleZ;
  float shiftX, shiftY, shiftZ;

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
    assert(positions[3 * v + 0] >= -1.0 - 0.001);
    assert(positions[3 * v + 0] <= 1.0 + 0.001);
    positions[3 * v + 1] = (positions[3 * v + 1] - shiftY) * scaleY;
    assert(positions[3 * v + 1] >= -1.0 - 0.001);
    assert(positions[3 * v + 1] <= 1.0 + 0.001);
    positions[3 * v + 2] = (positions[3 * v + 2] - shiftZ) * scaleZ;
    assert(positions[3 * v + 2] >= -1.0 - 0.001);
    assert(positions[3 * v + 2] <= 1.0 + 0.001);
  }
}

void Shape::init() {
  // Initialize the vertex array object
  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);

  // Send the position array to the GPU
  glGenBuffers(1, &positions_id);
  glBindBuffer(GL_ARRAY_BUFFER, positions_id);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0],
               GL_STATIC_DRAW);

  // Send the normal array to the GPU
  if (normals.empty()) {
    normals_id = 0;
  } else {
    glGenBuffers(1, &normals_id);
    glBindBuffer(GL_ARRAY_BUFFER, normals_id);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0],
                 GL_STATIC_DRAW);
  }

  // Send the texture array to the GPU
  if (texcoords.empty()) {
    texcoords_id = 0;
  } else {
    glGenBuffers(1, &texcoords_id);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_id);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), &texcoords[0],
                 GL_STATIC_DRAW);
  }

  // Send the element array to the GPU
  glGenBuffers(1, &indices_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // Unbind the arrays
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  assert(glGetError() == GL_NO_ERROR);
}

void Shape::draw(const std::shared_ptr<Program> prog) const {
  int h_pos, h_nor, h_tex;
  h_pos = h_nor = h_tex = -1;

  glBindVertexArray(vao_id);
  // Bind position buffer
  h_pos = prog->getAttribute("vertPos");
  GLSL::enableVertexAttribArray(h_pos);
  glBindBuffer(GL_ARRAY_BUFFER, positions_id);
  glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

  // Bind normal buffer
  h_nor = prog->getAttribute("vertNor");
  if (h_nor != -1 && normals_id != 0) {
    GLSL::enableVertexAttribArray(h_nor);
    glBindBuffer(GL_ARRAY_BUFFER, normals_id);
    glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
  }

  if (texcoords_id != 0) {
    // Bind texcoords buffer
    h_tex = prog->getAttribute("vertTex");
    if (h_tex != -1 && texcoords_id != 0) {
      GLSL::enableVertexAttribArray(h_tex);
      glBindBuffer(GL_ARRAY_BUFFER, texcoords_id);
      glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);
    }
  }

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

  // Draw
  glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT,
                 (const void*)0);

  // Disable and unbind
  if (h_tex != -1) {
    GLSL::disableVertexAttribArray(h_tex);
  }
  if (h_nor != -1) {
    GLSL::disableVertexAttribArray(h_nor);
  }
  GLSL::disableVertexAttribArray(h_pos);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::vector<float> Shape::GetPositions() {
  return positions;
}
