// Alex Ottoboni

#include "GameRenderer.h"

GameRenderer::GameRenderer() {}

GameRenderer::~GameRenderer() {}

GLFWwindow* GameRenderer::GetWindow() {
  return window;
}

void GameRenderer::Init(const std::string& resource_dir, 
                        std::shared_ptr<GameState> state,
                        GLFWerrorfun error_callback, 
                        GLFWkeyfun key_callback,
                        GLFWmousebuttonfun mouse_callback, 
                        GLFWframebuffersizefun resize_callback) {
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    std::cerr << "!glfwInit()" << std::endl;
    exit(1);
  }
  
  // request the highest possible version of OGL - important for mac
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  
  window = glfwCreateWindow(1600, 900, "Rhythm Runner", NULL, NULL);
  glfwMakeContextCurrent(window);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    exit(1);
  }

  glGetError(); // weird bootstrap of glGetError
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
  
  glfwSwapInterval(0); // vsync
  glfwSetKeyCallback(this->window, key_callback);
  glfwSetMouseButtonCallback(this->window, mouse_callback);
  glfwSetFramebufferSizeCallback(this->window, resize_callback);


  GLSL::checkVersion();
  glClearColor(.12f, .34f, .56f, 1.0f); // set background color
  glEnable(GL_DEPTH_TEST); // enable z-buffer test
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GameRenderer::Render(std::shared_ptr<GameState> game_state) {
  int width, height;
  glfwGetFramebufferSize(this->window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glfwSwapBuffers(this->window);
  glfwPollEvents();
}

void GameRenderer::Close() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
