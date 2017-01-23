#include "GlobalData.h"

unordered_map<int, function<void(int, int, int)>> GlobalData::keyMap;

void GlobalData::errorCallback(int error, const char *description) {
   cerr << description << endl;
}

void GlobalData::keyCallback(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
   if(GlobalData::keyMap.find(key) != GlobalData::keyMap.end()) {
      GlobalData::keyMap[key](scancode, action, mods);
   }
}

void GlobalData::insertCallback(int key, function<void(int, int, int)> function) {
   GlobalData::keyMap[key] = function;
}
