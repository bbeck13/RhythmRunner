#include "Octree.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <cstdlib>
#include "MovingObject.h"

Octree::Octree(
    std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects)
    : root(constructOctree(objects.get())) {
  this->objects = objects;
}

Octree::~Octree() {
  delete root;
}

Node* Octree::GetRoot() {
  return root;
}

std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> Octree::getObjects() {
  return objects;
}

Node* Octree::constructOctree(std::vector<std::shared_ptr<GameObject>>* objs) {
  // leaf
  if (objs->size() <= OBJS_IN_LEAF) {
    glm::vec3 min, max;
    if (objs->empty()) {
      std::cerr << "Catastrophic system failure. Unrecoverable" << std::endl;
      exit(EXIT_FAILURE);
    }
    return new Node(objs, AxisAlignedBox(getMin(objs), getMax(objs)),
                    new std::vector<Node*>());
  }

  glm::vec3 min = getMin(objs);
  glm::vec3 max = getMax(objs);
  AxisAlignedBox bb(min, max);

  std::vector<Node*>* children = new std::vector<Node*>();

  // TODO(bnbeck) profile quad tree vs octree
  for (AxisAlignedBox a : splitQuad(bb)) {
    std::vector<std::shared_ptr<GameObject>>* objects =
        getObjectsInBox(objs, a);
    if (!objects->empty()) {
      children->push_back(constructOctree(objects));
    }
  }

  return new Node(new std::vector<std::shared_ptr<GameObject>>(), bb, children);
}

// TODO(bnbeck) is it worth it to implement this?
// TODONE(bnbeck) yes, yes it is
std::vector<AxisAlignedBox> Octree::splitQuad(AxisAlignedBox toSplit) {
  std::vector<AxisAlignedBox> quadrants;

  // mid
  glm::vec3 mid_top_mid(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                        toSplit.GetMax().y,
                        0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 right_top_mid(toSplit.GetMax().x, toSplit.GetMax().y,
                          0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 left_bottom_mid(toSplit.GetMin().x, toSplit.GetMin().y,
                            0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 mid_bottom_mid(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                           toSplit.GetMin().y,
                           0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  // front
  glm::vec3 mid_bottom_front(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                             toSplit.GetMin().y, toSplit.GetMin().z);
  // back
  glm::vec3 mid_top_back(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                         toSplit.GetMax().y, toSplit.GetMax().z);

  quadrants.push_back(AxisAlignedBox(toSplit.GetMin(), mid_top_mid));
  quadrants.push_back(AxisAlignedBox(mid_bottom_front, right_top_mid));

  quadrants.push_back(AxisAlignedBox(left_bottom_mid, mid_top_back));
  quadrants.push_back(AxisAlignedBox(mid_bottom_mid, toSplit.GetMax()));

  return quadrants;
}

std::vector<AxisAlignedBox> Octree::splitOct(AxisAlignedBox toSplit) {
  std::vector<AxisAlignedBox> quadrants;

  // front points
  glm::vec3 mid_bottom_front(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                             toSplit.GetMin().y, toSplit.GetMin().z);
  glm::vec3 left_mid_front(toSplit.GetMin().x,
                           0.5f * (toSplit.GetMin().y + toSplit.GetMax().y),
                           toSplit.GetMin().z);
  glm::vec3 mid_mid_front(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                          0.5f * (toSplit.GetMin().y + toSplit.GetMax().y),
                          toSplit.GetMin().z);
  // middle points
  glm::vec3 left_bottom_mid(toSplit.GetMin().x, toSplit.GetMin().y,
                            0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 mid_bottom_mid(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                           toSplit.GetMin().y,
                           0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 left_mid_mid(toSplit.GetMin().x,
                         0.5f * (toSplit.GetMin().y + toSplit.GetMax().y),
                         0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 right_mid_mid(toSplit.GetMax().x,
                          0.5f * (toSplit.GetMin().y + toSplit.GetMax().y),
                          0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 mid_mid_mid(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                        0.5f * (toSplit.GetMin().y + toSplit.GetMax().y),
                        0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 mid_top_mid(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                        toSplit.GetMax().y,
                        0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  glm::vec3 right_top_mid(toSplit.GetMax().x, toSplit.GetMax().y,
                          0.5f * (toSplit.GetMin().z + toSplit.GetMax().z));
  // far points
  glm::vec3 mid_mid_back(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                         0.5f * (toSplit.GetMin().y + toSplit.GetMax().y),
                         toSplit.GetMax().z);
  glm::vec3 mid_top_back(0.5f * (toSplit.GetMin().x + toSplit.GetMax().x),
                         toSplit.GetMax().y, toSplit.GetMax().z);
  glm::vec3 right_mid_back(toSplit.GetMax().x,
                           0.5f * (toSplit.GetMin().y + toSplit.GetMax().y),
                           toSplit.GetMax().z);

  // front quadrants
  quadrants.push_back(AxisAlignedBox(toSplit.GetMin(), mid_mid_mid));
  quadrants.push_back(AxisAlignedBox(mid_bottom_front, right_mid_mid));
  quadrants.push_back(AxisAlignedBox(left_mid_front, mid_top_mid));
  quadrants.push_back(AxisAlignedBox(mid_mid_front, right_top_mid));

  // back quadrants
  quadrants.push_back(AxisAlignedBox(left_bottom_mid, mid_mid_back));
  quadrants.push_back(AxisAlignedBox(mid_bottom_mid, right_mid_back));
  quadrants.push_back(AxisAlignedBox(left_mid_mid, mid_top_back));
  quadrants.push_back(AxisAlignedBox(mid_mid_mid, toSplit.GetMax()));

  return quadrants;
}

std::vector<std::shared_ptr<GameObject>>* Octree::getObjectsInBox(
    std::vector<std::shared_ptr<GameObject>>* objs,
    AxisAlignedBox box) {
  std::vector<std::shared_ptr<GameObject>>* inBox =
      new std::vector<std::shared_ptr<GameObject>>();
  for (std::shared_ptr<GameObject> obj : *objs) {
    if (AxisAlignedBox::IsColliding(box, obj->GetBoundingBox())) {
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
