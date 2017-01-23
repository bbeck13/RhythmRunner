#ifndef __WORLD_H__
#define __WORLD_H__
#include "Window.h"
#include <vector>
#include "GameObject.h"

using namespace std;

class World {
public:
   World(Window *window);
   ~World();
private:
   Window *window;
   vector<GameObject> gameObjects;
};

#endif /*__WORD_H__*/
