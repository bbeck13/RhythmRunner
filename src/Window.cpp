#include "Window.h"

using namespace std;

int Window::height;
int Window::width;
GLFWwindow* Window::window;

Window::Window(int height, int width, string title) {
   Window::height = height;
   Window::width = width;

   glfwSetErrorCallback(GlobalData::errorCallback);
   if (!glfwInit()) {
      cerr << "Could not initialize glfw!" << endl;
      exit(EXIT_FAILURE);
   }
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

   Window::window = glfwCreateWindow(this->width, this->height, title.c_str(), NULL, NULL);

   if (!window) {
      glfwTerminate();
      cerr << "Could not initialize glfw window!" << endl;
      exit(EXIT_FAILURE);
   }

   glfwMakeContextCurrent(window);
   // Initialize GLEW.
   glewExperimental = true;
   if(glewInit() != GLEW_OK) {
      cerr << "Failed to initialize GLEW" << endl;
      exit(EXIT_FAILURE);
   }

   glGetError();
#ifdef DEBUG
   cerr << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cerr << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
#endif

   // Set vsync.
   glfwSwapInterval(1);
   // Set keyboard callback.
   glfwSetKeyCallback(window, GlobalData::keyCallback);

   glfwSetFramebufferSizeCallback(window, Window::resizeCallback);
}

Window::~Window() {
   glfwDestroyWindow(window);
}

void Window::resize(int width, int height) {
   Window::width = width;
   Window::height = height;
}

void Window::resizeCallback(GLFWwindow *window, int width, int height) {
   glViewport(0, 0, width, height);
   Window::resize(width, height);
}

bool Window::shouldClose() {
   return glfwWindowShouldClose(window);
}

