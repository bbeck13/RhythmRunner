// Joseph Arhar

#include "RendererSetup.h"

#include <iostream>

#include "GLSL.h"

GLFWwindow* RendererSetup::InitOpenGL() {
  if (!glfwInit()) {
    std::cerr << "!glfwInit()" << std::endl;
    exit(1);
  }

  // request the highest possible version of OGL - important for mac
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  GLFWwindow* window =
      glfwCreateWindow(mode->width, mode->height, TITLE, monitor, NULL);
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    exit(1);
  }
  glGetError();  // weird bootstrap of glGetError
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
  glfwSwapInterval(1);  // vsync

  GLSL::checkVersion();
  glClearColor(.12f, .34f, .56f, 1.0f);  // set background color
  glEnable(GL_DEPTH_TEST);               // enable z-buffer test
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ImGui_ImplGlfwGL3_Init(window, false);  // false -> don't install callbacks
  ImGuiIO& imgui_io = ImGui::GetIO();
  imgui_io.IniFilename = NULL;  // Disable imgui.ini
  imgui_io.Fonts->AddFontFromFileTTF(ASSET_DIR "/fonts/RobotoMono-Regular.ttf",
                                     24.0f);

  return window;
}

void RendererSetup::PreRender(GLFWwindow* window) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererSetup::PostRender(GLFWwindow* window) {
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void RendererSetup::Close(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void RendererSetup::ImGuiCenterWindow(double ratio) {
  ImGuiIO& imgui_io = ImGui::GetIO();
  int window_width = imgui_io.DisplaySize.x * ratio;
  int window_height = imgui_io.DisplaySize.y * ratio;
  ImGui::SetNextWindowSize(ImVec2(window_width, window_height),
                           ImGuiSetCond_FirstUseEver);
  ImGui::SetNextWindowPos(
      ImVec2(imgui_io.DisplaySize.x / 2 - window_width / 2,
             imgui_io.DisplaySize.y / 2 - window_height / 2),
      ImGuiSetCond_FirstUseEver);
}
