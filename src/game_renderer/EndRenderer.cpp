// Joseph Arhar

#include "EndRenderer.h"

#include "InputBindings.h"

EndRenderer::EndRenderer() {}

EndRenderer::~EndRenderer() {}

MainProgramMode EndRenderer::Render(GLFWwindow* window,
                                std::shared_ptr<GameState> game_state) {
  // Stay on the end screen until otherwise specified
  MainProgramMode program_mode = MainProgramMode::END_SCREEN;

  RendererSetup::PreRender(window);
  ImGui_ImplGlfwGL3_NewFrame();

  // Create a centered window that takes up 50% of the screen
  RendererSetup::ImGuiCenterWindow(0.5);
  ImGui::Begin("Game Over", NULL, RendererSetup::TITLE_WINDOW_FLAGS);

  ImGui::Text((std::string("Score: ") +
               std::to_string(game_state->GetPlayer()->GetScore()))
                  .c_str());

  if (ImGui::Button("Retry [SPACE]") ||
      InputBindings::KeyNewlyPressed(GLFW_KEY_SPACE)) {
    program_mode = MainProgramMode::GAME_SCREEN;
  }
  if (ImGui::Button("Main Menu [ENTER]") ||
      InputBindings::KeyNewlyPressed(GLFW_KEY_ENTER)) {
    program_mode = MainProgramMode::MENU_SCREEN;
  }
  if (ImGui::Button("Exit [ESCAPE]") ||
      InputBindings::KeyNewlyPressed(GLFW_KEY_ESCAPE)) {
    program_mode = MainProgramMode::EXIT;
  }

  ImGui::End();

  ImGui::Render();
  RendererSetup::PostRender(window);
  return program_mode;
}
