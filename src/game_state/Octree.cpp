#include "Octree.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <queue>
#include <ctime>
#include <tuple>
#include "MovingObject.h"
#include "DroppingPlatform.h"

Octree::Octree(
    std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects) {
  root = new Node(nullptr,
                  AxisAlignedBox(getMin(objects.get()), getMax(objects.get())),
                  new std::vector<Node*>());
  this->objects = objects;
  std::queue<std::pair<Node*, std::vector<std::shared_ptr<GameObject>>*>>
      toVisit;
  std::vector<std::shared_ptr<GameObject>>* objects_pair =
      new std::vector<std::shared_ptr<GameObject>>();
  for (std::shared_ptr<GameObject> obj : *objects) {
    objects_pair->push_back(obj);
  }
  toVisit.push(std::make_pair(root, objects_pair));
  while (!toVisit.empty()) {
    Node* node = toVisit.front().first;
    std::vector<std::shared_ptr<GameObject>>* objects_in_node =
        toVisit.front().second;
    toVisit.pop();
    if (objects_in_node->size() <= OBJS_IN_LEAF) {
      node->objects = objects_in_node;
      delete node->children;
      node->children = nullptr;
    } else {
      for (AxisAlignedBox b : node->boundingBox.split_quad()) {
        std::vector<std::shared_ptr<GameObject>>* objects_in_box =
            getObjectsInBox(objects_in_node, b);
        if (!objects_in_box->empty()) {
          Node* new_child = new Node(nullptr, b, new std::vector<Node*>());
          node->children->push_back(new_child);
          toVisit.push(std::make_pair(new_child, objects_in_box));
        }
      }
      delete objects_in_node;
    }
  }
}

Octree::~Octree() {
  delete root;
}

Node* Octree::GetRoot() {
  return root;
}

float Octree::GetKillZone() {
  return root->boundingBox.GetMin().y;
}

std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> Octree::getObjects() {
  return objects;
}

void Octree::insert(std::shared_ptr<GameObject> object) {
  std::queue<Node*> toVisit;
  AxisAlignedBox primary_object = object->GetBoundingBox();
  std::vector<std::shared_ptr<GameObject>>* new_object =
      new std::vector<std::shared_ptr<GameObject>>();
  new_object->push_back(object);
  if (std::shared_ptr<MovingObject> movingObj =
          std::dynamic_pointer_cast<MovingObject>(object)) {
    primary_object =
        movingObj->GetFullBox(object->GetModel(), object->GetScale());
  } else if (std::shared_ptr<gameobject::DroppingPlatform> droppingPlatform =
                 std::dynamic_pointer_cast<gameobject::DroppingPlatform>(
                     object)) {
    primary_object = droppingPlatform->GetFullBox();
  }

  toVisit.push(root);
  root->boundingBox = root->boundingBox.merge(primary_object);

  while (!toVisit.empty()) {
    Node* node = toVisit.front();
    toVisit.pop();
    if (AxisAlignedBox::IsColliding(node->boundingBox, primary_object)) {
      if (node->children != nullptr) {
        node->boundingBox = node->boundingBox.merge(primary_object);
        bool child_found = false;
        for (Node* child : *(node->children)) {
          if (AxisAlignedBox::IsColliding(child->boundingBox, primary_object)) {
            toVisit.push(child);
            child_found = true;
          }
        }
        if (!child_found) {
          Node* insertInto = node->children->at(0);
          float min_dist = primary_object.Distance(insertInto->boundingBox);
          for (Node* child : *(node->children)) {
            if (primary_object.Distance(child->boundingBox) < min_dist) {
              insertInto = child;
              min_dist = primary_object.Distance(insertInto->boundingBox);
            }
          }
          insertInto->boundingBox =
              insertInto->boundingBox.merge(primary_object);
          toVisit.push(insertInto);
        }
      } else {
        if (node->objects == nullptr) {
          std::cerr << "Hold up Nephew" << std::endl;
          exit(EXIT_FAILURE);
        }
        if (node->objects->size() < OBJS_IN_LEAF) {
          node->boundingBox = node->boundingBox.merge(primary_object);
          node->objects->push_back(object);
        } else {
          if (node->children == nullptr) {
            node->children = new std::vector<Node*>();
          }
          node->children->push_back(
              new Node(node->objects, node->boundingBox, nullptr));
          std::vector<std::shared_ptr<GameObject>>* new_obj =
              new std::vector<std::shared_ptr<GameObject>>();
          new_obj->push_back(object);
          node->children->push_back(
              new Node(new_object, primary_object, nullptr));
        }
      }
    }
  }
  objects->push_back(object);
}

void Octree::remove(std::shared_ptr<GameObject> object) {
  std::queue<Node*> toVisit;
  toVisit.push(root);
  AxisAlignedBox primary_object = object->GetBoundingBox();

  while (!toVisit.empty()) {
    Node* node = toVisit.front();
    toVisit.pop();
    if (node->children != nullptr) {
      for (Node* child : *(node->children)) {
        if (AxisAlignedBox::IsColliding(child->boundingBox, primary_object)) {
          toVisit.push(child);
        }
      }
    } else {
      auto it = std::find(node->objects->begin(), node->objects->end(), object);
      if (it != node->objects->end()) {
        // stackoverflow.com/honey_I_shrunk_the_kids
        std::swap(*it, node->objects->back());
        node->objects->pop_back();
      }
    }
  }

  auto it = std::find(objects->begin(), objects->end(), object);
  if (it != objects->end()) {
    std::swap(*it, objects->back());
    objects->pop_back();
  }
}

std::vector<std::shared_ptr<GameObject>>* Octree::getObjectsInBox(
    std::vector<std::shared_ptr<GameObject>>* objs,
    AxisAlignedBox box) {
  std::vector<std::shared_ptr<GameObject>>* inBox =
      new std::vector<std::shared_ptr<GameObject>>();
  for (std::shared_ptr<GameObject> obj : *objs) {
    AxisAlignedBox b = obj->GetBoundingBox();
    if (std::shared_ptr<MovingObject> movingObj =
            std::dynamic_pointer_cast<MovingObject>(obj)) {
      b = movingObj->GetFullBox(obj->GetModel(), obj->GetScale());
    } else if (std::shared_ptr<gameobject::DroppingPlatform> droppingPlatform =
                   std::dynamic_pointer_cast<gameobject::DroppingPlatform>(
                       obj)) {
      b = droppingPlatform->GetFullBox();
    }
    if (AxisAlignedBox::IsColliding(box, b)) {
      inBox->push_back(obj);
    }
  }
  return inBox;
}

glm::vec3 Octree::getMin(std::vector<std::shared_ptr<GameObject>>* objs) {
  glm::vec3 min(INFINITY, INFINITY, INFINITY);
  for (std::shared_ptr<GameObject> g : *objs) {
    glm::vec3 cur;
    if (std::shared_ptr<MovingObject> movingObj =
            std::dynamic_pointer_cast<MovingObject>(g)) {
      cur = movingObj->GetFullBox(g->GetModel(), g->GetScale()).GetMin();
    } else if (std::shared_ptr<gameobject::DroppingPlatform> droppingPlatform =
                   std::dynamic_pointer_cast<gameobject::DroppingPlatform>(g)) {
      cur = droppingPlatform->GetFullBox().GetMin();
    } else {
      cur = g->GetBoundingBox().GetMin();
    }

    if (min.x > cur.x) {
      min.x = cur.x;
    }
    if (min.y > cur.y) {
      min.y = cur.y;
    }
    if (min.z > cur.z) {
      min.z = cur.z;
    }
  }
  return min;
}

glm::vec3 Octree::getMax(std::vector<std::shared_ptr<GameObject>>* objs) {
  glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);
  for (std::shared_ptr<GameObject> g : *objs) {
    glm::vec3 cur;
    if (std::shared_ptr<MovingObject> movingObj =
            std::dynamic_pointer_cast<MovingObject>(g)) {
      cur = movingObj->GetFullBox(g->GetModel(), g->GetScale()).GetMax();
    } else if (std::shared_ptr<gameobject::DroppingPlatform> droppingPlatform =
                   std::dynamic_pointer_cast<gameobject::DroppingPlatform>(g)) {
      cur = droppingPlatform->GetFullBox().GetMax();
    } else {
      cur = g->GetBoundingBox().GetMax();
    }

    if (max.x < cur.x) {
      max.x = cur.x;
    }
    if (max.y < cur.y) {
      max.y = cur.y;
    }
    if (max.z < cur.z) {
      max.z = cur.z;
    }
  }
  return max;
}

std::string Octree::ToString() {
  return ToString(root);
}

std::string Octree::ToString(Node* n) {
  std::stringstream tree;
  tree << "Node: " << n->boundingBox.ToString() << std::endl;
  tree << "Leafs: {" << std::endl;
  for (std::shared_ptr<GameObject> g : *n->objects) {
    tree << g->GetBoundingBox().ToString() << std::endl;
  }
  tree << "}" << std::endl;
  tree << "Branches {" << std::endl;
  for (Node* child : *n->children) {
    tree << ToString(child) << std::endl;
  }
  tree << "}" << std::endl;
  return tree.str();
}
