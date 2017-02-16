// Joseph Arhar

#ifndef MENU_RENDERER_H_
#define MENU_RENDERER_H_

#include <memory>

#include "MenuState.h"
#include "ProgramMode.h"
#include "RendererSetup.h"

class MenuRenderer {
 public:
  MenuRenderer();
  ~MenuRenderer();

  ProgramMode Render(GLFWwindow* window, std::shared_ptr<MenuState> menu_state);
};

#endif
