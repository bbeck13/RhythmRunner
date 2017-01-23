#include "World.h"

World::World(Window *window) {
   this->window = window;
   //gameObjects.add();
}

World::~World() {
   delete window;
}
