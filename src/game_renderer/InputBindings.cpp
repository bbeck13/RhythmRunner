// Joseph Arhar

#include "InputBindings.h"

#include "Logging.h"

static ImGuiIO prev_imgui_io;
static ImGuiIO new_imgui_io;

InputBindings::InputBindings() {}

InputBindings::~InputBindings() {}

void InputBindings::Bind(GLFWwindow* window) {
  glfwSetErrorCallback(InputBindings::ErrorCallback);
  glfwSetKeyCallback(window, InputBindings::KeyCallback);
  glfwSetMouseButtonCallback(window, InputBindings::MouseCallback);
  glfwSetFramebufferSizeCallback(window, InputBindings::ResizeCallback);
  glfwSetCursorPosCallback(window, InputBindings::CursorCallback);

  glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);
  glfwSetCharCallback(window, ImGui_ImplGlfwGL3_CharCallback);
}

void InputBindings::StoreKeypresses() {
  prev_imgui_io = new_imgui_io;
  new_imgui_io = ImGui::GetIO();
}

bool InputBindings::KeyNewlyPressed(int key) {
  return !prev_imgui_io.KeysDown[key] && new_imgui_io.KeysDown[key];
}

void InputBindings::KeyCallback(GLFWwindow* window,
                                int key,
                                int scancode,
                                int action,
                                int mods) {
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
