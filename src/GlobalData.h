#ifndef __GLOBAL_DATA_H__
#define __GLOBAL_DATA_H__
#include <unordered_map>
#include <iostream>
#include <functional>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

class GlobalData {
public:
   static void errorCallback(int error, const char *description);
   static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
   static void insertCallback(int key, function<void(int, int, int)>);

private:
   static unordered_map<int, function<void(int, int, int)>> keyMap;
};

#endif /*__GLOBAL_DATA_H__*/
