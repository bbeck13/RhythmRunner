// Joseph Arhar

#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <vector>

// A Model is a mesh imported from a .obj file
class Model {
 public:
  Model(const std::string& filename);
  ~Model();

  const std::string& GetFilename();
  // TODO(jarhar): consider fixing up renderer and making these const (not ref)
  std::vector<unsigned>& GetElements();
  std::vector<float>& GetPositions();
  std::vector<float>& GetNormals();
  std::vector<float>& GetTextureCoordinates();

 private:
  void Normalize();

  const std::string filename;
  std::vector<unsigned> elements;
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> texture_coordinates;
};

#endif
