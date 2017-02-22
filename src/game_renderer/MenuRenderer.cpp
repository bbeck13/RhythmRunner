// Joseph Arhar

#include "MenuRenderer.h"

#include <string.h>

#include "InputBindings.h"

#define TEXT_FIELD_LENGTH 256

MenuRenderer::MenuRenderer() {}

MenuRenderer::~MenuRenderer() {}

ProgramMode MenuRenderer::Render(GLFWwindow* window,
                                 std::shared_ptr<MenuState> menu_state) {
  // stay on the main menu until otherwise specified
  ProgramMode program_mode = ProgramMode::MENU_SCREEN;

  RendererSetup::PreRender(window);
  ImGui_ImplGlfwGL3_NewFrame();

  // Create a centered window that takes up 50% of the screen
  RendererSetup::ImGuiCenterWindow(0.5);
  ImGui::Begin("Rhythm Runner", NULL, RendererSetup::TITLE_WINDOW_FLAGS);

  static char music_path_buffer[TEXT_FIELD_LENGTH];
  strncpy(music_path_buffer, menu_state->GetMusicPath().c_str(),
          TEXT_FIELD_LENGTH);
  if (ImGui::InputText("Music Filepath", music_path_buffer, TEXT_FIELD_LENGTH)) {
    // TODO(jarhar): validate music filepath here
    menu_state->SetMusicPath(std::string(music_path_buffer, TEXT_FIELD_LENGTH));
  }
  if (ImGui::Button("Start [ENTER]") ||
      InputBindings::KeyNewlyPressed(GLFW_KEY_ENTER)) {
    // TODO(jarhar): validate music filepath here
    program_mode = ProgramMode::GAME_SCREEN;
  }
  if (ImGui::Button("Exit [ESCAPE]") ||
      InputBindings::KeyNewlyPressed(GLFW_KEY_ESCAPE)) {
    program_mode = ProgramMode::EXIT;
  }

  ImGui::End();

  ImGui::Render();
  RendererSetup::PostRender(window);
  return program_mode;
}
