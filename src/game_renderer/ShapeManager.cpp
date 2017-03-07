// Joseph Arhar

#include <map>

#include "ShapeManager.h"

static bool opengl_initialized = false;
static std::map<std::string, std::shared_ptr<Shape>> path_to_shape;

std::shared_ptr<Shape> ShapeManager::GetShape(const std::string& path) {
  auto iterator = path_to_shape.find(path);
  if (iterator != path_to_shape.end()) {
    return iterator->second;
  }

  std::shared_ptr<Shape> shape = std::make_shared<Shape>();
  shape->loadMesh(std::string(ASSET_DIR "/") + path);
  if (opengl_initialized) {
    shape->init();
  }

  path_to_shape[path] = shape;
  return shape;
}

void ShapeManager::InitGL() {
  opengl_initialized = true;
  for (auto& iterator : path_to_shape) {
    iterator.second->init();
  }
}
