// Joseph Arhar

#ifndef GAME_RENDERER_H_
#define GAME_RENDERER_H_

#include <unordered_map>
#include <unordered_set>

#include "GameState.h"
#include "GLFW/glfw3.h"
#include "Program.h"
#include "MatrixStack.h"
#include "GameCamera.h"

#define TITLE "Rhythm Runner"
#define PLATFORM_PROG "platform_prog"

class GameRenderer {
 public:
  GameRenderer();
  ~GameRenderer();

  void Init(const std::string& resource_dir);
  void InitState(std::shared_ptr<GameState> state);
  void Render(std::shared_ptr<GameState> game_state);
  void Close();
  bool WindowShouldClose();

 private:
  void ImGuiInit();
  void ImGuiRender(std::shared_ptr<GameState> game_state);

  GLFWwindow* window;
  std::unordered_map<std::string, std::shared_ptr<Program>> programs;
  std::shared_ptr<Program> ProgramFromJSON(std::string filepath);
  static std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
  GetObjectsInView(std::shared_ptr<std::vector<glm::vec4>> vfplane,
                   std::shared_ptr<Octree> tree);
};

#endif
