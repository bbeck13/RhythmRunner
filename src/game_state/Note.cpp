#include <memory>
#include "Note.h"
#include <iostream>

std::shared_ptr<Shape> Note::shape = std::make_shared<Shape>();
bool Note::isInitialized = false;

Note::Note(glm::vec3 position) : GameObject(Note::note) {
  this->position = position;
  // todo play around with the size
  this->scale = glm::vec3(0.5, 0.5, 0.5);
  this->model = note;
}

Note::Note(glm::vec3 position, glm::vec3 scale) : GameObject(Note::note) {
  this->position = position;
  // todo play around with the size
  this->scale = scale;
  this->model = note;
}

Note::~Note() {}

std::shared_ptr<Shape> Note::GetNoteShape() {
  if (!isInitialized) {
    shape->loadMesh(PLATFORM_MESH);
    shape->init();
    isInitialized = true;
  }
  return shape;
}
