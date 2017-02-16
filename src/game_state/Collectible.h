#ifndef COLLECTIBLE_H_
#define COLLECTIBLE_H_
#include "GameObject.h"
#include "Shape.h"

class Collectible : public GameObject {
 public:
  Collectible(std::shared_ptr<Shape> shape);
  ~Collectible();

  virtual void SetCollected();
  virtual void SetUncollected();
  virtual bool GetCollected() const;

  ObjectType GetType() override;

 protected:
  bool isCollected;
};

#endif
