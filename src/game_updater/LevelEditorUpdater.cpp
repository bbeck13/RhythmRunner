#include "LevelEditorUpdater.h"

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>

#include "GameCamera.h"
#include "ProgramMode.h"
#include "InputBindings.h"

#define CURSOR_IN_FRONT 8
#define MOUSE_MOVE_SENSITIVITY 0.01f

LevelEditorUpdater::LevelEditorUpdater() {}

LevelEditorUpdater::~LevelEditorUpdater() {}

LevelProgramMode LevelEditorUpdater::Update(
    std::shared_ptr<GameState> game_state) {
  if (InputBindings::KeyPressed(GLFW_KEY_Q)) {
    return LevelProgramMode::EXIT;
  }

  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  float camera_move = game_state->GetLevelEditorState()->GetCameraMove();
  float camera_yaw_move = game_state->GetLevelEditorState()->GetCameraYawMove();
  int width, height;
  glfwGetWindowSize(game_state->GetWindow(), &width, &height);
  bool cursor_locked =
      InputBindings::GetCursorMode() == InputBindings::CursorMode::LOCKED;

  if (InputBindings::KeyPressed(GLFW_KEY_ESCAPE)) {
    InputBindings::SetCursorMode(cursor_locked
                                     ? InputBindings::CursorMode::FREE
                                     : InputBindings::CursorMode::LOCKED);
    cursor_locked =
        InputBindings::GetCursorMode() == InputBindings::CursorMode::LOCKED;
  }

  if (cursor_locked) {
    // only update position and camera in cursor locked mode
    bool scrimshaw = false;  // Call me Ishmael

    // update camera with cursor
    std::pair<double, double> cursor_diff = InputBindings::GetCursorDiff();
    if (std::abs(cursor_diff.first) > MOUSE_MOVE_SENSITIVITY ||
        std::abs(cursor_diff.second) > MOUSE_MOVE_SENSITIVITY) {
      camera->pivot(width, height, (width / 2.0f) - cursor_diff.first,
                    (height / 2.0f) - cursor_diff.second);
      scrimshaw = true;
    }

    // update camera with keys
    if (InputBindings::KeyDown(GLFW_KEY_L) ||
        InputBindings::KeyDown(GLFW_KEY_RIGHT)) {
      camera->pivot(width, height, (width / 2.0f) - camera_yaw_move,
                    height / 2.0f);
      scrimshaw = true;
    }
    if (InputBindings::KeyDown(GLFW_KEY_H) ||
        InputBindings::KeyDown(GLFW_KEY_LEFT)) {
      camera->pivot(width, height, (width / 2.0f) + camera_yaw_move,
                    height / 2.0f);
      scrimshaw = true;
    }
    if (InputBindings::KeyDown(GLFW_KEY_K) ||
        InputBindings::KeyDown(GLFW_KEY_UP)) {
      camera->pivot(width, height, width / 2.0f,
                    (height / 2.0f) - camera_yaw_move);
      scrimshaw = true;
    }
    if (InputBindings::KeyDown(GLFW_KEY_J) ||
        InputBindings::KeyDown(GLFW_KEY_DOWN)) {
      camera->pivot(width, height, width / 2.0f,
                    (height / 2.0f) + camera_yaw_move);
      scrimshaw = true;
    }

    glm::vec3 camera_position = camera->getPosition();
    glm::vec3 look_at = camera->getLookAt();
    glm::vec3 view = camera_position - look_at;
    glm::vec3 strafe = cross(glm::vec3(0, 1, 0), view);

    if (InputBindings::KeyDown(GLFW_KEY_W)) {
      camera_position -= camera_move * view;
      look_at -= camera_move * view;
    }
    if (InputBindings::KeyDown(GLFW_KEY_S)) {
      camera_position += camera_move * view;
      look_at += camera_move * view;
    }
    if (InputBindings::KeyDown(GLFW_KEY_A)) {
      camera_position -= camera_move * strafe;
      look_at -= camera_move * strafe;
    }
    if (InputBindings::KeyDown(GLFW_KEY_D)) {
      camera_position += camera_move * strafe;
      look_at += camera_move * strafe;
    }

    if (scrimshaw) {
      look_at += CURSOR_IN_FRONT * view;
    } else {
      game_state->GetPlayer()->SetPosition(
          game_state->GetCamera()->getLookAt());
    }

    if (InputBindings::KeyDown(GLFW_KEY_E)) {
      game_state->GetLevelEditorState()->SetCameraMove(camera_move * 1.2);
      game_state->GetLevelEditorState()->SetCameraYawMove(camera_yaw_move *
                                                          1.2);
    }
    if (InputBindings::KeyDown(GLFW_KEY_C)) {
      game_state->GetLevelEditorState()->SetCameraMove(camera_move * 0.6);
      game_state->GetLevelEditorState()->SetCameraYawMove(camera_yaw_move *
                                                          0.6);
    }

    camera->setPosition(camera_position);
    camera->setLookAt(look_at);
  }

  return LevelProgramMode::EDIT_LEVEL;
}
