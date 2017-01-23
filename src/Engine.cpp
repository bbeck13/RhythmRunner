#include "Engine.h"

Engine::Engine(World *world) {
   this->world = world;
}

Engine::~Engine() {
   delete world;
}

void Engine::init() {
}

void Engine::run() {
}
