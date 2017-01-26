// Alex Ottoboni

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "GameRenderer.h"
#include "GLSL.h"
#include "GL/glew.h"
#include "json.hpp"
#include "MatrixStack.h"
#include "../helpers/FileSystemUtils.h"

GameRenderer::GameRenderer() {}

GameRenderer::~GameRenderer() {}

GLFWwindow* GameRenderer::GetWindow() {
  return window;
}

std::shared_ptr<Program> GameRenderer::ProgramFromJSON(std::string filepath) {
  // Read in the file
  std::ifstream json_input_stream (filepath, std::ifstream::in);
  // Read the file into the JSON library
  nlohmann::json json_handler;
  json_input_stream >> json_handler;
  
  // Get name attributes from JSON
  std::string vert_name = json_handler["vert"];
  std::string frag_name = json_handler["frag"];
  std::string prog_name = json_handler["name"];

  // Create new shader program
  std::shared_ptr<Program> new_program;
  new_program = std::make_shared<Program>();
  new_program->setVerbose(true);
  new_program->setName(prog_name);
  new_program->setShaderNames("../assets/shaders/" + vert_name, 
                              "../assets/shaders/" + frag_name);
  new_program->init();

  // Create the uniforms
  std::vector<std::string> uniforms = json_handler["uniforms"];
  for (int i = 0; i < uniforms.size(); i++) {
    new_program->addUniform(uniforms[i]);
  }

  // Create the attributes
  std::vector<std::string> attributes = json_handler["attributes"];
  for (int i = 0; i < attributes.size(); i++) {
    new_program->addAttribute(uniforms[i]);
  }

  return new_program;
}

void GameRenderer::Init(const std::string& resource_dir, 
                        std::shared_ptr<GameState> state,
                        GLFWerrorfun error_callback, 
                        GLFWkeyfun key_callback,
                        GLFWmousebuttonfun mouse_callback, 
                        GLFWframebuffersizefun resize_callback) {

  // OpenGL Setup Boilerplate
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

  // Initialize all programs from JSON files in assets folder
  std::shared_ptr<Program> temp_program;
  std::vector<std::string> json_files = FileSystemUtils::ListFiles("../assets/shaders/*.json");
  for (int i = 0; i < json_files.size(); i++) {
    temp_program = GameRenderer::ProgramFromJSON(json_files[i]);  
    programs[temp_program->getName()] = temp_program;
  } 
}

void GameRenderer::Render(std::shared_ptr<GameState> game_state) {
  int width, height;
  glfwGetFramebufferSize(this->window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render Loop Goes Here

  glfwSwapBuffers(this->window);
  glfwPollEvents();
}

void GameRenderer::Close() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
