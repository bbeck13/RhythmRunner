#include "LevelEditorMenuRenderer.h"

#include <string.h>

#include "InputBindings.h"

#define TEXT_FIELD_LENGTH 256

LevelEditorMenuRenderer::LevelEditorMenuRenderer() {}

LevelEditorMenuRenderer::~LevelEditorMenuRenderer() {}

LevelProgramMode LevelEditorMenuRenderer::Render(
    GLFWwindow* window,
    std::shared_ptr<MenuState> menu_state) {
  // stay on the main menu until otherwise specified
  LevelProgramMode program_mode = LevelProgramMode::MENU_SCREEN;

  RendererSetup::PreRender(window);
  ImGui_ImplGlfwGL3_NewFrame();

  // Create a centered window that takes up 50% of the screen
  RendererSetup::ImGuiCenterWindow(0.5);
  ImGui::Begin("Rhythm Runner Level Editor", NULL,
               RendererSetup::TITLE_WINDOW_FLAGS);

  static char music_path_buffer[TEXT_FIELD_LENGTH];
  static char level_path_buffer[TEXT_FIELD_LENGTH];
  strncpy(music_path_buffer, menu_state->GetMusicPath().c_str(),
          TEXT_FIELD_LENGTH);

  strncpy(level_path_buffer, menu_state->GetLevelPath().c_str(),
          TEXT_FIELD_LENGTH);

  if (ImGui::InputText("Music Filepath", music_path_buffer,
                       TEXT_FIELD_LENGTH)) {
    menu_state->SetMusicPath(std::string(music_path_buffer, TEXT_FIELD_LENGTH));
  }

  if (ImGui::InputText("Level Filepath", level_path_buffer,
                       TEXT_FIELD_LENGTH)) {
    menu_state->SetLevelPath(std::string(level_path_buffer, TEXT_FIELD_LENGTH));
  }

  if (ImGui::Button("Generate Level From Music [ENTER]") ||
      InputBindings::KeyNewlyPressed(GLFW_KEY_ENTER)) {
    program_mode = LevelProgramMode::GENERATE_LEVEL;
  }
  if (ImGui::Button("Level Editor")) {
    // TODO bbeck13 do
  }
  if (ImGui::Button("Exit [ESCAPE]") ||
      InputBindings::KeyNewlyPressed(GLFW_KEY_ESCAPE)) {
    program_mode = LevelProgramMode::EXIT;
  }

  ImGui::End();

  ImGui::Render();
  RendererSetup::PostRender(window);
  return program_mode;
}
