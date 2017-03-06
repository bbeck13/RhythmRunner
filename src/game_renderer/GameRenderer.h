// Joseph Arhar

#ifndef GAME_RENDERER_H_
#define GAME_RENDERER_H_

#include <unordered_map>
#include <unordered_set>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GameCamera.h"
#include "GameState.h"
#include "MatrixStack.h"
#include "Program.h"

#define PLATFORM_PROG "platform_prog"

struct Light {
  glm::vec4 position;
  glm::vec3 intensities;
};

class GameRenderer {
 public:
  GameRenderer();
  ~GameRenderer();

  void Init(const std::string& resource_dir);
  void Render(GLFWwindow* window, std::shared_ptr<GameState> game_state);
  void RenderLevelEditor(GLFWwindow* window,
                         std::shared_ptr<GameState> game_state);
  static std::shared_ptr<Program> ProgramFromJSON(std::string filepath);
  static std::shared_ptr<Texture> TextureFromJSON(std::string filepath);
  static std::unordered_set<std::shared_ptr<GameObject>>* GetObjectsInView(
      std::shared_ptr<std::vector<glm::vec4>> vfplane,
      std::shared_ptr<Octree> tree);

 private:
  void RenderIt(GLFWwindow* window,
                std::shared_ptr<GameState> game_state,
                bool render_level_editor);
  void ImGuiRender(std::shared_ptr<GameState> game_state, bool level_editor);
  void LevelEditorRenderer(std::shared_ptr<GameState> game_state);
  std::unordered_map<std::string, std::shared_ptr<Program>> programs;
  std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
  std::vector<glm::vec3> color_vec;
};

#endif
