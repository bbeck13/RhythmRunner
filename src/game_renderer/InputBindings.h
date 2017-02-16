// Joseph Arhar

#ifndef INPUT_BINDINGS_H_
#define INPUT_BINDINGS_H_

#include <memory>

#include "RendererSetup.h"

class InputBindings {
 public:
  static void Bind(GLFWwindow* window);

  // These are used to tell if the player "pressed" a button based on whether or
  // not they were already pressing it during the last frame
  static void StoreKeypresses();
  static bool KeyNewlyPressed(int key);

 private:
  InputBindings();
  ~InputBindings();

  static void KeyCallback(GLFWwindow* window,
                          int key,
                          int scancode,
                          int action,
                          int mods);
  static void MouseCallback(GLFWwindow* window,
                            int button,
                            int action,
                            int mods);
  static void ErrorCallback(int error, const char* description);
  static void ResizeCallback(GLFWwindow* window, int width, int height);
  static void CursorCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif
