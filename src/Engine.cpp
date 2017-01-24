#include "Engine.h"
#include "World.h"

unordered_map<int, function<void(int, int, int)>> Engine::keyMap;
string Engine::resourceDir;

Engine::Engine(World *world, string resourceDir) {
   this->world = world;
   this->resourceDir = resourceDir;
}

string Engine::getResourceDir() {
   return resourceDir;
}

Engine::~Engine() {
   delete world;
}

void Engine::init() {
   GLSL::checkVersion();
   world->init();
}

void Engine::run() {
   world->update();
}


void Engine::errorCallback(int error, const char *description) {
   cerr << description << endl;
}

void Engine::keyCallback(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
#ifdef DEBUG
   cerr << "Got key " << key << endl;
#endif
   if(Engine::keyMap.find(key) != Engine::keyMap.end()) {
      Engine::keyMap[key](scancode, action, mods);
   }
}

void Engine::insertCallback(int key, function<void(int, int, int)> function) {
   Engine::keyMap[key] = function;
}
