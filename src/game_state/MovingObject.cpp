#include "MovingObject.h"
#include "GameObject.h"

MovingObject::MovingObject(std::shared_ptr<std::vector<glm::vec3>> path,
                           glm::vec3 position,
                           float velocity)
    : path(path) {
  originalPosition = position;
  currentDir = 0;
  this->origionalPath = path;
  this->velocity = glm::vec3(velocity, velocity, velocity);
  this->origionalVelocity = this->velocity;
  if (path->size() > 0) {
    movementVector = calculateMovementVector(path->at(0), position);
  } else {
    movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
  }
}

MovingObject::MovingObject(std::shared_ptr<std::vector<glm::vec3>> path,
                           glm::vec3 position,
                           glm::vec3 velocity)
    : path(path) {
  originalPosition = position;
  currentDir = 0;
  this->origionalPath = path;
  this->velocity = velocity;
  this->origionalVelocity = this->velocity;
  if (path->size() > 0) {
    movementVector = calculateMovementVector(path->at(0), position);
  } else {
    movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
  }
}

void MovingObject::Reset() {
  currentDir = 0;
  velocity = origionalVelocity;
  if (path->size() > 0) {
    movementVector = calculateMovementVector(path->at(0), originalPosition);
  } else {
    movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
  }
}

glm::vec3 MovingObject::GetOriginalPosition() {
  return originalPosition;
}

glm::vec3 MovingObject::GetMovementVector() {
  return movementVector;
}

glm::vec3 MovingObject::calculateMovementVector(glm::vec3 goal,
                                                glm::vec3 start) {
  glm::vec3 moveVector;
  moveVector.x = goal.x - start.x;
  moveVector.y = goal.y - start.y;
  moveVector.z = goal.z - start.z;

  float hyp =
      std::sqrt((moveVector.x * moveVector.x) + (moveVector.y * moveVector.y) +
                (moveVector.z * moveVector.z));
  moveVector.x /= hyp;
  moveVector.y /= hyp;
  moveVector.z /= hyp;

  return moveVector;
}

glm::vec3 MovingObject::updatePosition(glm::vec3 position) {
  glm::vec3 newPosition;

  newPosition.x = position.x + movementVector.x * velocity.x;
  newPosition.y = position.y + movementVector.y * velocity.y;
  newPosition.z = position.z + movementVector.z * velocity.z;

  if (path->size() > 0) {
    // if we moved farther from the goal position rather than closer
    if (distance(path->at(currentDir), newPosition) >
        distance(path->at(currentDir), position)) {
      currentDir++;
      if (currentDir == (int)path->size()) {
        currentDir = 0;
      }
      movementVector =
          calculateMovementVector(path->at(currentDir), newPosition);
    }
  }

  return newPosition;
}

glm::vec3 MovingObject::GetVelocity() {
  return velocity;
}

std::shared_ptr<std::vector<glm::vec3>> MovingObject::GetPath() {
  return path;
}

void MovingObject::SetVelocity(float velocity) {
  this->velocity = glm::vec3(velocity, velocity, velocity);
}

AxisAlignedBox MovingObject::GetFullBox(std::shared_ptr<Shape> model,
                                        glm::vec3 scale) {
  AxisAlignedBox box(model, scale, originalPosition);

  for (glm::vec3 pos : *path) {
    box = box.merge(AxisAlignedBox(model, scale, pos));
  }
  return box;
}

void MovingObject::SetPath(std::shared_ptr<std::vector<glm::vec3>> path) {
  this->path = path;
}

double MovingObject::distance(glm::vec3 one, glm::vec3 two) {
  float xDistance = one.x - two.x;
  float yDistance = one.y - two.y;
  float zDistance = one.z - two.z;
  float hypotenuse = sqrt((xDistance * xDistance) + (yDistance * yDistance) +
                          (zDistance * zDistance));
  return hypotenuse;
}
