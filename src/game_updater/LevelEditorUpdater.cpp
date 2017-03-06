#include "LevelEditorUpdater.h"

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>

#include "GameCamera.h"
#include "ProgramMode.h"

#define CURSOR_IN_FRONT 8

LevelEditorUpdater::LevelEditorUpdater() {}
LevelEditorUpdater::~LevelEditorUpdater() {}

LevelProgramMode LevelEditorUpdater::Update(
    std::shared_ptr<GameState> game_state) {
  LevelProgramMode mode = LevelProgramMode::EDIT_LEVEL;
  UpdateCamera(game_state);
  if (ImGui::GetIO().KeysDown[GLFW_KEY_Q]) {
    mode = LevelProgramMode::EXIT;
  }
  return mode;
}
void LevelEditorUpdater::Init(std::shared_ptr<GameState> game_state) {}

void LevelEditorUpdater::UpdateLevel(std::shared_ptr<GameState> game_state) {}

void LevelEditorUpdater::UpdateCamera(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  bool scrimshaw = false;  // Call me Ishmael
  float camera_move = game_state->GetLevelEditorState()->GetCameraMove();
  float camera_yaw_move = game_state->GetLevelEditorState()->GetCameraYawMove();
  int width, height;
  glfwGetWindowSize(game_state->GetWindow(), &width, &height);

  if (ImGui::GetIO().KeysDown[GLFW_KEY_D]) {
    camera->pivot(width, height, (width / 2.0f) - camera_yaw_move,
                  height / 2.0f);
    scrimshaw = true;
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_A]) {
    camera->pivot(width, height, (width / 2.0f) + camera_yaw_move,
                  height / 2.0f);
    scrimshaw = true;
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_W]) {
    camera->pivot(width, height, width / 2.0f,
                  (height / 2.0f) - camera_yaw_move);
    scrimshaw = true;
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_S]) {
    camera->pivot(width, height, width / 2.0f,
                  (height / 2.0f) + camera_yaw_move);
    scrimshaw = true;
  }

  glm::vec3 camera_position = camera->getPosition();
  glm::vec3 look_at = camera->getLookAt();
  glm::vec3 view = camera_position - look_at;
  glm::vec3 strafe = cross(glm::vec3(0, 1, 0), view);

  // don't worry guys it's vim 👌
  if (ImGui::GetIO().KeysDown[GLFW_KEY_J]) {
    camera_position += camera_move * view;
    look_at += camera_move * view;
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_K]) {
    camera_position -= camera_move * view;
    look_at -= camera_move * view;
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_H]) {
    camera_position -= camera_move * strafe;
    look_at -= camera_move * strafe;
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_L]) {
    camera_position += camera_move * strafe;
    look_at += camera_move * strafe;
  }
  if (scrimshaw) {
    look_at += CURSOR_IN_FRONT * view;
  } else {
    game_state->GetPlayer()->SetPosition(game_state->GetCamera()->getLookAt());
  }

  if (ImGui::GetIO().KeysDown[GLFW_KEY_UP]) {
    game_state->GetLevelEditorState()->SetCameraMove(camera_move * 1.2);
    game_state->GetLevelEditorState()->SetCameraYawMove(camera_yaw_move * 1.2);
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_DOWN]) {
    game_state->GetLevelEditorState()->SetCameraMove(camera_move * 0.6);
    game_state->GetLevelEditorState()->SetCameraYawMove(camera_yaw_move * 0.6);
  }

  camera->setPosition(camera_position);
  camera->setLookAt(look_at);
}
