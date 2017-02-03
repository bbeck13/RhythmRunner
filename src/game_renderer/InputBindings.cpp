// Joseph Arhar

#include "InputBindings.h"

#include "Logging.h"

static std::shared_ptr<GameState> static_game_state;

InputBindings::InputBindings() {}

InputBindings::~InputBindings() {}

void InputBindings::Init(std::shared_ptr<GameState> game_state,
                         GLFWwindow* window) {
  static_game_state = game_state;

  glfwSetErrorCallback(InputBindings::ErrorCallback);
  glfwSetKeyCallback(window, InputBindings::KeyCallback);
  glfwSetMouseButtonCallback(window, InputBindings::MouseCallback);
  glfwSetFramebufferSizeCallback(window, InputBindings::ResizeCallback);
  glfwSetCursorPosCallback(window, InputBindings::CursorCallback);
}

void InputBindings::Close() {
  static_game_state.reset();
}

void InputBindings::KeyCallback(GLFWwindow* window,
                                int key,
                                int scancode,
                                int action,
                                int mods) {
  switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GL_TRUE);
      break;
    case GLFW_KEY_SPACE:
      static_game_state->GetPlayer()->SetSpacebarDown(action != GLFW_RELEASE);
      break;
  }
}

void InputBindings::MouseCallback(GLFWwindow* window,
                                  int button,
                                  int action,
                                  int mods) {
  double posX, posY;
  if (action == GLFW_PRESS) {
    glfwGetCursorPos(window, &posX, &posY);
  }
}

void InputBindings::ErrorCallback(int error, const char* description) {
  LOG_ERROR(description);
}

void InputBindings::ResizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void InputBindings::CursorCallback(GLFWwindow* window,
                                   double xpos,
                                   double ypos) {
  // camera->pivot(WINDOW_WIDTH, WINDOW_HEIGHT, xpos, ypos);
  // glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
}
