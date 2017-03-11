// Joseph Arhar

#ifndef INPUT_BINDINGS_H_
#define INPUT_BINDINGS_H_

#include <memory>

#include "RendererSetup.h"

class InputBindings {
 public:
  static void Bind(GLFWwindow* window);

  static bool KeyDown(int key);
  static bool KeyPressed(int key);
  static void ClearKeyPresses();

  enum CursorMode { FREE = 1, LOCKED = 2 };
  static void SetCursorMode(CursorMode cursor_mode);
  static CursorMode GetCursorMode();
  static std::pair<float, float> GetCursorDiff();

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
