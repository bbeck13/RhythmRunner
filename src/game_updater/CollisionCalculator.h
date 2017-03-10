// Joseph Arhar

#ifndef COLLISION_CALCULATOR_H_
#define COLLISION_CALCULATOR_H_

#include <memory>
#include <unordered_set>

#include "GameObject.h"
#include "Octree.h"

namespace CollisionCalculator {
std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
GetCollidingObjects(AxisAlignedBox primary_object,
                    std::shared_ptr<Octree> tree);
}

#endif  // COLLISION_CALCULATOR_H_
