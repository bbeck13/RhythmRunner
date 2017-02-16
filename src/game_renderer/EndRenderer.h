// Joseph Arhar

#ifndef END_RENDERER_H_
#define END_RENDERER_H_

#include <memory>

#include "RendererSetup.h"
#include "ProgramMode.h"
#include "GameState.h"

class EndRenderer {
 public:
  EndRenderer();
  ~EndRenderer();

  ProgramMode Render(GLFWwindow* window, std::shared_ptr<GameState> game_state);
};

#endif
