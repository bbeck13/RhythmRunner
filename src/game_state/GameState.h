// Joseph Arhar

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GameObject.h"
#include "Level.h"
#include "GameCamera.h"
#include "Player.h"
#include "VideoTexture.h"
#include "Sky.h"
#include "LevelEditorState.h"
#include "Particles.h"

class GameState {
 public:
  GameState(std::shared_ptr<Level> level,
            std::shared_ptr<GameCamera> camera,
            std::shared_ptr<Player> player,
            std::shared_ptr<Sky> sky,
            GLFWwindow* window);
  ~GameState();

  std::shared_ptr<Level> GetLevel();
  std::shared_ptr<GameCamera> GetCamera();
  std::shared_ptr<Player> GetPlayer();
  std::shared_ptr<Sky> GetSky();
  std::unordered_map<std::string, std::shared_ptr<VideoTexture>>
  GetVideoTextures();
  std::unordered_set<std::shared_ptr<GameObject>>* GetObjectsInView();

  bool Done();
  uint64_t GetElapsedTicks();
  uint64_t GetStartTick();
  uint64_t GetMusicStartTick();
  double GetStartTime();
  std::shared_ptr<LevelEditorState> GetLevelEditorState();
  GLFWwindow* GetWindow();

  void AddVideoTexture(std::string name, std::shared_ptr<VideoTexture> texture);
  void SetLevel(std::shared_ptr<Level> level);
  void SetCamera(std::shared_ptr<GameCamera> camera);
  void SetDone(bool done);
  void IncrementTicks();
  void SetStartTime();
  void SetItemsInView(std::unordered_set<std::shared_ptr<GameObject>>* objects);
  void SetLevelEditorState(
      std::shared_ptr<LevelEditorState> level_editor_state);
 private:
  std::shared_ptr<Level> level;
  std::shared_ptr<GameCamera> camera;
  std::shared_ptr<Player> player;
  std::shared_ptr<Sky> sky;
  std::unordered_map<std::string, std::shared_ptr<VideoTexture>> video_textures;
  std::unordered_set<std::shared_ptr<GameObject>>* objectsInView;
  std::shared_ptr<LevelEditorState> level_editor_state;
  GLFWwindow* window;

  bool done;
  uint64_t elapsed_ticks;
  uint64_t start_tick;  // value of elapsed_ticks when game started
  double start_time;    // value of glfwGetTime() when game started
  uint64_t music_end_tick;
};

#endif
