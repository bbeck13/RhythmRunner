#include "LevelEditorMenuRenderer.h"

#include <string.h>

#include "InputBindings.h"
#include "FileSystemUtils.h"

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
  RendererSetup::ImGuiCenterWindow(0.5, RendererSetup::DYNAMIC);
  ImGui::Begin("Rhythm Runner Level Editor", NULL,
               RendererSetup::DYNAMIC_WINDOW_FLAGS);

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
  if (!FileSystemUtils::FileExists(menu_state->GetMusicPath())) {
    ImGui::TextColored(ImColor(255, 0, 0), "Music file not found");
  } else {
    ImGui::TextColored(ImColor(0, 255, 0), "Music file found");
  }

  if (ImGui::InputText("Level Filepath", level_path_buffer,
                       TEXT_FIELD_LENGTH)) {
    menu_state->SetLevelPath(std::string(level_path_buffer, TEXT_FIELD_LENGTH));
  }
  if (!FileSystemUtils::FileExists(menu_state->GetLevelPath())) {
    ImGui::TextColored(ImColor(255, 0, 0),
                       "Level file not found (need to generate)");
  } else {
    ImGui::TextColored(ImColor(0, 255, 0), "Level file found");
  }

  if (ImGui::Button("Generate Level File From Music [ENTER]") ||
      InputBindings::KeyPressed(GLFW_KEY_ENTER)) {
    if (FileSystemUtils::FileExists(menu_state->GetMusicPath())) {
      program_mode = LevelProgramMode::GENERATE_LEVEL;
    }
  }
  if (ImGui::Button("Level Editor [TAB]") ||
      InputBindings::KeyPressed(GLFW_KEY_TAB)) {
    if (FileSystemUtils::FileExists(menu_state->GetLevelPath())) {
      program_mode = LevelProgramMode::START_EDIT_LEVEL;
    }
  }
  if (ImGui::Button("Exit [ESCAPE][Q]") ||
      InputBindings::KeyPressed(GLFW_KEY_ESCAPE) ||
      InputBindings::KeyPressed(GLFW_KEY_Q)) {
    program_mode = LevelProgramMode::EXIT;
  }

  if (program_mode != LevelProgramMode::MENU_SCREEN) {
    ImGui::TextColored(ImColor(0, 255, 0), "Loading...");
  }

  ImGui::End();

  ImGui::Render();
  RendererSetup::PostRender(window);
  return program_mode;
}
