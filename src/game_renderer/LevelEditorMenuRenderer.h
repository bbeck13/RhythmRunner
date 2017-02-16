// Braden Beck

#ifndef LEVEL_EDITOR_MENU_RENDERER_H_
#define LEVEL_EDITOR_MENU_RENDERER_H_

#include <memory>

#include "MenuState.h"
#include "ProgramMode.h"
#include "RendererSetup.h"

class LevelEditorMenuRenderer {
 public:
  LevelEditorMenuRenderer();
  ~LevelEditorMenuRenderer();

  LevelProgramMode Render(GLFWwindow* window, std::shared_ptr<MenuState> menu_state);
};

#endif
