// Joseph Arhar

#include "MenuRenderer.h"

#include <string.h>

#include "InputBindings.h"

#define TEXT_FIELD_LENGTH 256

MenuRenderer::MenuRenderer() {}

MenuRenderer::~MenuRenderer() {}

MainProgramMode MenuRenderer::Render(GLFWwindow* window,
                                     std::shared_ptr<MenuState> menu_state) {
  // stay on the main menu until otherwise specified
  MainProgramMode program_mode = MainProgramMode::MENU_SCREEN;

  RendererSetup::PreRender(window);
  ImGui_ImplGlfwGL3_NewFrame();

  // Create a centered window that takes up 50% of the screen
  RendererSetup::ImGuiCenterWindow(0.5, RendererSetup::DYNAMIC);
  ImGui::Begin("Rhythm Runner", NULL, RendererSetup::DYNAMIC_WINDOW_FLAGS);

  static char music_path_buffer[TEXT_FIELD_LENGTH];
  static char level_path_buffer[TEXT_FIELD_LENGTH];
  strncpy(music_path_buffer, menu_state->GetMusicPath().c_str(),
          TEXT_FIELD_LENGTH);

  strncpy(level_path_buffer, menu_state->GetLevelPath().c_str(),
          TEXT_FIELD_LENGTH);

  if (ImGui::InputText("Music Filepath", music_path_buffer,
                       TEXT_FIELD_LENGTH)) {
    // TODO(jarhar): validate music filepath here
    menu_state->SetMusicPath(std::string(music_path_buffer, TEXT_FIELD_LENGTH));
  }

  if (ImGui::InputText("Level Filepath", level_path_buffer,
                       TEXT_FIELD_LENGTH)) {
    // TODO(jarhar): validate music filepath here
    menu_state->SetLevelPath(std::string(level_path_buffer, TEXT_FIELD_LENGTH));
  }

  if (ImGui::Button("Start [ENTER]") ||
      InputBindings::KeyPressed(GLFW_KEY_ENTER)) {
    // TODO(jarhar): validate music filepath here
    program_mode = MainProgramMode::CREATE_NEW_GAME;
  }

  if (ImGui::Button("Exit [ESCAPE][Q]") ||
      InputBindings::KeyPressed(GLFW_KEY_ESCAPE) ||
      InputBindings::KeyPressed(GLFW_KEY_Q)) {
    program_mode = MainProgramMode::EXIT;
  }

  ImGui::End();

  ImGui::Render();
  RendererSetup::PostRender(window);
  return program_mode;
}
