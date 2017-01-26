// Joseph Arhar

#ifndef GAME_RENDERER_H_
#define GAME_RENDERER_H_

#include <iostream>
#include <cmath>
#include "Program.h"
#include "GLFW/glfw3.h"
#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLSL.h"
#include "MatrixStack.h"
#include "Texture.h"
#include "game_state/GameState.h"

class GameRenderer {
 public:
  GameRenderer();
  ~GameRenderer();

  GLFWwindow* GetWindow();
  
  void Init(const std::string& resource_dir, std::shared_ptr<GameState> state,
            GLFWerrorfun error_callback, 
            GLFWkeyfun key_callback,
            GLFWmousebuttonfun mouse_callback, 
            GLFWframebuffersizefun resize_callback);
  void Render(std::shared_ptr<GameState> game_state);

  void Close();
 private:
  GLFWwindow* window;
};



#endif
