// Joseph Arhar

#include "MenuRenderer.h"

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <iostream>
#include <string.h>

#define TEXT_FIELD_LENGTH 256

MenuRenderer::MenuRenderer() {}

MenuRenderer::~MenuRenderer() {}

void MenuRenderer::Render(GLFWwindow* window,
                          std::shared_ptr<MenuState> menu_state) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static const ImGuiWindowFlags window_flags;

  ImGuiIO& imgui_io = ImGui::GetIO();
  if (imgui_io.KeysDown[GLFW_KEY_ESCAPE]) {
    std::cout << "escape down" << std::endl;
  } else {
    std::cout << "escape not down" << std::endl;
  }

  ImGui_ImplGlfwGL3_NewFrame();

  ImGui::Begin("Rhythm Runner", NULL, window_flags);
  ImGui::Text("Music Filepath:");
  static char music_path_buffer[TEXT_FIELD_LENGTH];
  strncpy(music_path_buffer, menu_state->GetMusicPath().c_str(),
          TEXT_FIELD_LENGTH);
  if (ImGui::InputText("", music_path_buffer, TEXT_FIELD_LENGTH)) {
    // TODO(jarhar): validate music filepath here
    menu_state->SetMusicPath(std::string(music_path_buffer, TEXT_FIELD_LENGTH));
  }
  if (ImGui::Button("Start [ENTER]") || imgui_io.KeysDown[GLFW_KEY_ENTER]) {
    // TODO(jarhar): validate music filepath here
    // signal for the game to start
    menu_state->SetMenuMode(MenuMode::START_GAME);
  }
  ImGui::End();

  ImGui::Render();

  glfwSwapBuffers(window);
  glfwPollEvents();
}
