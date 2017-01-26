// Joseph Arhar

#ifndef GAME_RENDERER_H_
#define GAME_RENDERER_H_

#include <cmath>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glob.h>
#include <iostream>
#include <unordered_map>
#include "game_state/GameState.h"
#include "GLFW/glfw3.h"
#include "GL/glew.h"
#include "GLSL.h"
#include "json.hpp"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"

class GameRenderer {
 public:
  GameRenderer();
  ~GameRenderer();
  GLFWwindow* GetWindow();
  std::shared_ptr<Program> ProgramFromJSON(std::string filepath);
  std::vector<std::string> ListFiles(const std::string& pattern);
  void Init(const std::string& resource_dir, std::shared_ptr<GameState> state,
            GLFWerrorfun error_callback, 
            GLFWkeyfun key_callback,
            GLFWmousebuttonfun mouse_callback, 
            GLFWframebuffersizefun resize_callback);
  void Render(std::shared_ptr<GameState> game_state);
  void Close();
 private:
  GLFWwindow* window;
  std::unordered_map<std::string, std::shared_ptr<Program>> programs;
};

#endif
