// Joseph Arhar

#ifndef RENDERER_SETUP_H_
#define RENDERER_SETUP_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#define TITLE "Rhythm Runner"

class RendererSetup {
 public:
  static GLFWwindow* InitOpenGL();
  static void Close(GLFWwindow* window);
  static void PreRender(GLFWwindow* window);
  static void PostRender(GLFWwindow* window);

  static void ImGuiCenterWindow(double ratio, bool dynamic);
  static void ImGuiTopRightCornerWindow(double ratio, bool dynamic);
  static void ImGuiTopLeftCornerWindow(double ratio, bool dynamic);
  static void ImGuiBottomRightCornerWindow(double ratio, bool dynamic);
  static void ImGuiBottomLeftCornerWindow(double ratio, bool dynamic);

  static const ImGuiWindowFlags STATIC_WINDOW_FLAGS =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;
  static const ImGuiWindowFlags DYNAMIC_WINDOW_FLAGS = 0;

  static const bool STATIC = false;
  static const bool DYNAMIC = true;

 private:
  RendererSetup();
  virtual ~RendererSetup();
};

#endif
