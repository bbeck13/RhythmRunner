// Joseph Arhar

#ifndef MENU_RENDERER_H_
#define MENU_RENDERER_H_

#include <memory>
#include <GLFW/glfw3.h>

#include "MenuState.h"

class MenuRenderer {
 public:
  MenuRenderer();
  ~MenuRenderer();

  void Render(GLFWwindow* window, std::shared_ptr<MenuState> menu_state);
};

#endif
