#include "Window.h"

using namespace std;

int Window::height;
int Window::width;
GLFWwindow* Window::window;
bool Window::close = false;

Window::Window(int height, int width, string title) {
   Window::height = height;
   Window::width = width;
   Window::close = false;

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

   glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   glfwSetCursorPos(window, width / 2.0f, height / 2.0f);
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

void Window::swapBuffers() {
   glfwSwapBuffers(window);
}

bool Window::shouldClose() {
   return glfwWindowShouldClose(window) || close;
}

float Window::getAspect() {
   return width / height;
}

void Window::setClose(bool toSet) {
   Window::close = toSet;
}
