// Joseph Arhar

#ifndef INPUT_BINDINGS_H_
#define INPUT_BINDINGS_H_

#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class InputBindings {
 public:
  static void Bind(GLFWwindow* window);

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
