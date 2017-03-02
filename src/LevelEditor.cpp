#include <fstream>
#include <iostream>

#include "RendererSetup.h"
#include "GameObject.h"
#include "Platform.h"
#include "Note.h"
#include "LevelGenerator.h"
#include "DroppingPlatform.h"
#include "MovingPlatform.h"
#include "json.hpp"
#include "LevelJson.h"
#include "InputBindings.h"
#include "LevelEditorMenuRenderer.h"
#include "RendererSetup.h"

#define MUSIC "music/2.wav"

int main(int argc, char** argv) {
  GLFWwindow* window = RendererSetup::InitOpenGL();
  InputBindings::Bind(window);

  LevelEditorMenuRenderer menu_renderer;
  std::shared_ptr<MenuState> menu_state = std::make_shared<MenuState>();
  LevelProgramMode program_mode = LevelProgramMode::MENU_SCREEN;

  while (!glfwWindowShouldClose(window)) {
    switch (program_mode) {

      case LevelProgramMode::MENU_SCREEN: {
        program_mode = menu_renderer.Render(window, menu_state);
        break;
      }

      case LevelProgramMode::GENERATE_LEVEL: {
        if (!menu_state->GetMusicPath().empty() &&
            !menu_state->GetLevelPath().empty()) {
          LevelGenerator level_generator(menu_state->GetMusicPath());
          std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> level =
              level_generator.Generate();
          nlohmann::json j = *level;
          std::ofstream output(menu_state->GetLevelPath());
          output << j;
        }
        program_mode = LevelProgramMode::MENU_SCREEN;
        break;
      }

      case LevelProgramMode::EXIT:
        RendererSetup::Close(window);
        return EXIT_SUCCESS;
    }
  }
  RendererSetup::Close(window);
  return EXIT_SUCCESS;
}
