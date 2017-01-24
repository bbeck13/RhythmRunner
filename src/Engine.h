#ifndef __ENGINE_H__
#define __ENGINE_H__
#include <unordered_map>
#include <iostream>
#include <functional>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"

using namespace std;

class World;

class Engine {
public:
   Engine(World *world, string resourceDir);
   ~Engine();
   void init();
   void run();
   static void errorCallback(int error, const char *description);
   static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                           int mods);
   static void insertCallback(int key, function<void(int, int, int)>);
   static string getResourceDir();
private:
   World *world;
   static string resourceDir;
   static unordered_map<int, function<void(int, int, int)>> keyMap;
};

#endif /*__ENGINE_H__*/
