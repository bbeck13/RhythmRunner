// Joseph Arhar

#include "InputBindings.h"

#include "Logging.h"

static GLFWwindow* static_window;
static bool key_pressed_buffer[512];
static InputBindings::CursorMode cursor_mode = InputBindings::CursorMode::FREE;
static std::pair<double, double> last_cursor_pos;

InputBindings::InputBindings() {}

InputBindings::~InputBindings() {}

void InputBindings::Bind(GLFWwindow* window) {
  static_window = window;

  glfwSetErrorCallback(InputBindings::ErrorCallback);
  glfwSetKeyCallback(window, InputBindings::KeyCallback);
  glfwSetMouseButtonCallback(window, InputBindings::MouseCallback);
  glfwSetFramebufferSizeCallback(window, InputBindings::ResizeCallback);
  glfwSetCursorPosCallback(window, InputBindings::CursorCallback);

  glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);
  glfwSetCharCallback(window, ImGui_ImplGlfwGL3_CharCallback);

  SetCursorMode(CursorMode::FREE);
}

bool InputBindings::KeyPressed(int key) {
  if (key_pressed_buffer[key]) {
    // "handle" the keypress at this moment
    key_pressed_buffer[key] = false;
    return true;
  }
  return false;
}

bool InputBindings::KeyDown(int key) {
  // handle the keypress if there was one
  key_pressed_buffer[key] = false;
  return ImGui::GetIO().KeysDown[key];
}

void InputBindings::ClearKeyPresses() {
  std::fill(std::begin(key_pressed_buffer), std::end(key_pressed_buffer),
            false);
}

void InputBindings::SetCursorMode(CursorMode new_cursor_mode) {
  if (cursor_mode == new_cursor_mode) {
    return;
  }
  cursor_mode = new_cursor_mode;

  if (cursor_mode == CursorMode::LOCKED) {
    glfwSetInputMode(static_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double xpos, ypos;
    glfwGetCursorPos(static_window, &xpos, &ypos);
    last_cursor_pos = std::pair<double, double>(xpos, ypos);
  } else {
    glfwSetInputMode(static_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

InputBindings::CursorMode InputBindings::GetCursorMode() {
  return cursor_mode;
}

std::pair<float, float> InputBindings::GetCursorDiff() {
  if (cursor_mode != CursorMode::LOCKED) {
    return std::pair<float, float>(0, 0);
  }

  double xpos, ypos;
  glfwGetCursorPos(static_window, &xpos, &ypos);
  std::pair<float, float> diff = std::pair<double, double>(
      xpos - last_cursor_pos.first, ypos - last_cursor_pos.second);
  last_cursor_pos = std::pair<double, double>(xpos, ypos);
  return diff;
}

void InputBindings::KeyCallback(GLFWwindow* window,
                                int key,
                                int scancode,
                                int action,
                                int mods) {
  if (action == GLFW_PRESS && key < 512) {
    key_pressed_buffer[key] = true;
  }
  ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}

void InputBindings::MouseCallback(GLFWwindow* window,
                                  int button,
                                  int action,
                                  int mods) {
  double posX, posY;
  if (action == GLFW_PRESS) {
    glfwGetCursorPos(window, &posX, &posY);
  }

  ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
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
