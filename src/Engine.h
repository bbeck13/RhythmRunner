#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "World.h"

class Engine {
public:
   Engine(World *world);
   ~Engine();
   void init();
   void run();
private:
   World *world;
};

#endif /*__ENGINE_H__*/
