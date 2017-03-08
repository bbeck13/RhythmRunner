// Joseph Arhar

#include <queue>

#include "CollisionCalculator.h"

std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
CollisionCalculator::GetCollidingObjects(AxisAlignedBox primary_object,
                    std::shared_ptr<Octree> tree) {
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>> collisions =
      std::make_shared<std::unordered_set<std::shared_ptr<GameObject>>>();
  std::queue<Node*> toVisit;
  toVisit.push(tree->GetRoot());

  while (!toVisit.empty()) {
    Node* node = toVisit.front();
    toVisit.pop();
    if (node->objects->empty()) {
      for (Node* child : *(node->children)) {
        if (AxisAlignedBox::IsColliding(child->boundingBox, primary_object)) {
          if (!(child->objects->empty() && child->children->empty())) {
            toVisit.push(child);
          }
        }
      }
    } else {
      for (std::shared_ptr<GameObject> objectInBox : *(node->objects)) {
        if (AxisAlignedBox::IsColliding(objectInBox->GetBoundingBox(),
                                        primary_object)) {
          collisions->insert(objectInBox);
        }
      }
    }
  }
  return collisions;
}
