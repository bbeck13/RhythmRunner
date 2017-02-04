#include "Note.h"
#include <memory>
#include <iostream>

std::shared_ptr<Shape> Note::shape = std::make_shared<Shape>();
bool Note::isInitialized = false;

Note::Note(glm::vec3 position) : GameObject(Note::shape) {
  this->position = position;
  // todo play around with the size
  this->scale = glm::vec3(0.5, 0.5, 0.5);
  this->model = shape;
  this->isCollected = false;
}

Note::Note(glm::vec3 position, glm::vec3 scale) : GameObject(Note::shape) {
  this->position = position;
  this->scale = scale;
  this->model = shape;
  this->isCollected = false;
}

Note::~Note() {}

void Note::SetCollected() {
  this->isCollected = true;
}

void Note::SetUncollected() {
  this->isCollected = false;
}

bool Note::GetCollected() {
  return this->isCollected;
}

std::shared_ptr<Shape> Note::GetModel() {
  if (!Note::isInitialized) {
    Note::shape->loadMesh(std::string(ASSET_DIR) + "/" + std::string(NOTE_MESH));
    Note::shape->init();
    Note::isInitialized = true;
  }
  return Note::shape;
}
