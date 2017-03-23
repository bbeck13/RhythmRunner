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
#include "ProgramMode.h"
#include "ParticleGenerator.h"

#define PLATFORM_PROG "platform_prog"

struct Light {
  glm::vec4 position;
  glm::vec3 intensities;
};

class GameRenderer {
 public:
  GameRenderer();
  ~GameRenderer();

  void Init(const std::string& resource_dir, GLFWwindow* window);
  MainProgramMode RenderCameraSetup(GLFWwindow* window,
                                    std::shared_ptr<GameState> game_state);
  MainProgramMode Render(GLFWwindow* window,
                         std::shared_ptr<GameState> game_state);
  LevelProgramMode RenderLevelEditor(GLFWwindow* window,
                                     std::shared_ptr<GameState> game_state);

  static std::shared_ptr<Program> ProgramFromJSON(std::string filepath);
  static std::shared_ptr<Texture> TextureFromJSON(std::string filepath);
  static std::unordered_set<std::shared_ptr<GameObject>>* GetObjectsInView(
      std::shared_ptr<std::vector<glm::vec4>> vfplane,
      std::shared_ptr<Octree> tree);
  void DrawPhysicalObjectTree(
      std::shared_ptr<Program> program,
      MatrixStack P,
      MatrixStack V,
      std::shared_ptr<PhysicalObject> physical_object) const;

  static void InitBloom(int height, int width);
  void Bloom(int height, int width);
  void SetBloom(bool doBloom);

 private:
  static std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>
  GetObjectsOfType(std::unordered_set<std::shared_ptr<GameObject>>* objects,
                   SecondaryType type_to_get);

  void RenderObjects(GLFWwindow* window, std::shared_ptr<GameState> game_state);
  void RenderSingleObject(std::shared_ptr<GameObject> object,
                          std::shared_ptr<Program> program,
                          std::shared_ptr<Texture> texture,
                          std::shared_ptr<MatrixStack> P,
                          std::shared_ptr<MatrixStack> V);
  void RenderSingleObject(std::shared_ptr<GameObject> object,
                          std::shared_ptr<MatrixStack> P,
                          std::shared_ptr<MatrixStack> V);
  void RenderLevelCollectibles(
      std::unordered_set<std::shared_ptr<GameObject>>* objects,
      SecondaryType type_to_render,
      std::shared_ptr<MatrixStack> P,
      std::shared_ptr<MatrixStack> V);
  void RenderLevelCollectibles(
      std::unordered_set<std::shared_ptr<GameObject>>* objects,
      SecondaryType type_to_render,
      glm::vec3 color,
      std::shared_ptr<MatrixStack> P,
      std::shared_ptr<MatrixStack> V);
  void RenderLevelObjects(
      std::unordered_set<std::shared_ptr<GameObject>>* objects,
      SecondaryType type_to_render,
      std::shared_ptr<Texture> video_texture,
      std::shared_ptr<MatrixStack> P,
      std::shared_ptr<MatrixStack> V);
  void RenderParticles(std::shared_ptr<ParticleGenerator> particles,
                       std::shared_ptr<MatrixStack> P,
                       std::shared_ptr<MatrixStack> V);
  void RenderLevelObjects(
      std::unordered_set<std::shared_ptr<GameObject>>* objects,
      SecondaryType type_to_render,
      std::shared_ptr<MatrixStack> P,
      std::shared_ptr<MatrixStack> V);
  void RenderLevel(GLFWwindow* window, std::shared_ptr<GameState> game_state);
  void RenderDepth(GLFWwindow* window, std::shared_ptr<GameState> game_state);

  void RenderMinimap(GLFWwindow* window, std::shared_ptr<GameState> game_state);
  void ImGuiRenderBegin(std::shared_ptr<GameState> game_state);
  void ImGuiRenderEnd();
  void RenderQuad();
  MainProgramMode ImGuiRenderGame(std::shared_ptr<GameState> game_state);
  LevelProgramMode ImGuiRenderEditor(std::shared_ptr<GameState> game_state);

  static std::unordered_map<std::string, std::shared_ptr<Program>> programs;
  std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
  std::vector<glm::vec3> color_vec;

  static GLuint hdrFBO;
  static GLuint hdrColorBuffers[2];
  static GLuint pingpongFBO[2];
  static GLuint pingpongColorbuffers[2];
  static GLuint rboDepth;
  GLuint quadVAO = 0;
  GLuint quadVBO;
  GLboolean bloom = true;
  glm::vec3 light_dir = glm::vec3(1, 1, 1);
  glm::mat4 LSpace;
};

#endif
