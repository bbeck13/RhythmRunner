#ifndef OCTREE_H
#define OCTREE_H
#include <string>
#include <vector>
#include "GameObject.h"
#include "AxisAlignedBox.h"

// TODO(bnbeck) play around with this value it can change the performace a lot
#define OBJS_IN_LEAF 25

class Node {
 public:
  std::vector<std::shared_ptr<GameObject>>* objects;
  AxisAlignedBox boundingBox;
  std::vector<Node*>* children;

  Node(std::vector<std::shared_ptr<GameObject>>* items,
       AxisAlignedBox bb,
       std::vector<Node*>* childs)
      : objects(items), boundingBox(bb), children(childs) {}
  ~Node() {
    for (Node* n : *children) {
      delete n;
    }
    delete children;
  }

 private:
};

class Octree {
 public:
  Octree(std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects);
  ~Octree();

  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> getObjects();
  std::string ToString();
  float GetKillZone();

  void insert(std::shared_ptr<GameObject> object);
  void remove(std::shared_ptr<GameObject> object);

  Node* GetRoot();

 private:
  Node* root;
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects;
  void prune();

  static std::string ToString(Node* n);

  static Node* constructOctree(std::vector<std::shared_ptr<GameObject>>* objs);
  static std::vector<std::shared_ptr<GameObject>>* getObjectsInBox(
      std::vector<std::shared_ptr<GameObject>>* objs,
      AxisAlignedBox box);
  static glm::vec3 getMin(std::vector<std::shared_ptr<GameObject>>* objs);
  static glm::vec3 getMax(std::vector<std::shared_ptr<GameObject>>* objs);
  static std::vector<AxisAlignedBox> splitOct(AxisAlignedBox toSplit);
  static std::vector<AxisAlignedBox> splitQuad(AxisAlignedBox toSplit);
};

#endif
