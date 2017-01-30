// Joseph Arhar

#ifndef GAME_RENDERER_H_
#define GAME_RENDERER_H_

#include <unordered_map>
#include "game_state/GameState.h"
#include "GLFW/glfw3.h"
#include "Program.h"
#include "MatrixStack.h"
#include "GameCamera.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480
#define TITLE "Rhythm Runner"
#define MUSIC "../assets/music/1.wav"
#define PLATFORM_PROG "platform_prog"

class GameRenderer {
 public:
  GameRenderer();
  ~GameRenderer();
  GLFWwindow* GetWindow();
  void Init(const std::string& resource_dir,
            std::shared_ptr<GameState> state,
            GLFWerrorfun error_callback,
            GLFWkeyfun key_callback,
            GLFWmousebuttonfun mouse_callback,
            GLFWframebuffersizefun resize_callback);
  void Render(std::shared_ptr<GameState> game_state);
  void Close();

 private:
  GLFWwindow* window;
  std::unordered_map<std::string, std::shared_ptr<Program>> programs;
  std::shared_ptr<Program> ProgramFromJSON(std::string filepath);
  std::shared_ptr<GameCamera> Camera;
  MatrixStack Perspective;
  MatrixStack Model;
};

#endif
