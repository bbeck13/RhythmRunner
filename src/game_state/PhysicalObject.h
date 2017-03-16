// Joseph Arhar

#ifndef PHYSICAL_OBJECT_H_
#define PHYSICAL_OBJECT_H_

#include "Texture.h"
#include "AxisAlignedBox.h"
#include "MatrixStack.h"

class PhysicalObject {
 public:
  PhysicalObject(const std::string& shape_path,
                 glm::vec3 position = glm::vec3(0, 0, 0),
                 glm::vec3 rotation_axis = glm::vec3(0, 1, 0),
                 float rotation_angle = 0,
                 glm::vec3 scale = glm::vec3(1, 1, 1));
  virtual ~PhysicalObject();

  glm::vec3 GetPosition() const;
  glm::vec3 GetRotationAxis() const;
  float GetRotationAngle() const;
  virtual glm::mat4 GetRotationMatrix()
      const;  // for objects with complex rotations
  glm::vec3 GetScale() const;
  std::shared_ptr<Texture> GetTexture() const;  // Is null/empty if no texture
  std::shared_ptr<Shape> GetModel() const;
  std::shared_ptr<Program> GetProgram() const;
  glm::mat4 GetTransform() const;  // This accounts for the object hierarchy
  AxisAlignedBox
  GetIndividualBoundingBox();       // AABB of this mesh with transform
  AxisAlignedBox GetBoundingBox();  // AABB surrounding entire tree
  std::vector<std::shared_ptr<PhysicalObject>> GetSubObjects();

  void SetPosition(glm::vec3 position);
  void SetRotationAxis(glm::vec3 rotation_axis);
  void SetRotationAngle(float rotation_angle);
  void SetScale(glm::vec3 scale);
  void SetTexture(std::shared_ptr<Texture> texture);

  void AddSubObject(std::shared_ptr<PhysicalObject> sub_object);

 protected:
  std::shared_ptr<Shape> shape;
  glm::vec3 position;
  glm::vec3 rotation_axis;
  float rotation_angle;
  glm::vec3 scale;
  std::shared_ptr<Texture> texture;
  std::shared_ptr<Program> program;

  // cached data
  AxisAlignedBox bounding_box;
  bool bounding_box_dirty;

  // all sub physical objects represent a tree and have location data relative
  // to their parent
  std::vector<std::shared_ptr<PhysicalObject>> sub_objects;
  PhysicalObject* parent_object;  // TODO(jarhar): find a better way to do this
};

#endif  // PHYSICAL_OBJECT_H_
