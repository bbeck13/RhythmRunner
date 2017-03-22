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
#include "SoundEffects.h"
#include "ParticleGenerator.h"

class GameState {
 public:
  enum PlayingState { PLAYING, PAUSED, FAILURE, SUCCESS };

  GameState(std::shared_ptr<Level> level,
            std::shared_ptr<GameCamera> camera,
            std::shared_ptr<Player> player,
            std::shared_ptr<Sky> sky,
            GLFWwindow* window,
            std::shared_ptr<ParticleGenerator> particles =
                std::shared_ptr<ParticleGenerator>());
  virtual ~GameState();

  std::shared_ptr<Level> GetLevel();
  std::shared_ptr<GameCamera> GetCamera();
  std::shared_ptr<Player> GetPlayer();
  std::shared_ptr<Sky> GetSky();
  std::shared_ptr<ParticleGenerator> GetParticles();
  std::unordered_map<std::string, std::shared_ptr<VideoTexture>>
  GetVideoTextures();
  std::unordered_set<std::shared_ptr<GameObject>>* GetObjectsInView();

  uint64_t GetElapsedTicks();
  uint64_t GetStartTick();
  uint64_t GetMusicStartTick();
  double GetStartTime();
  std::shared_ptr<LevelEditorState> GetLevelEditorState();
  GLFWwindow* GetWindow();
  PlayingState GetPlayingState();
  uint64_t GetEndingTick();
  double GetEndingTime();
  double GetProgressRatio();
  bool ReachedEndOfLevel();
  SoundEffects GetSoundEffects();

  void AddVideoTexture(std::string name, std::shared_ptr<VideoTexture> texture);
  void SetLevel(std::shared_ptr<Level> level);
  void SetCamera(std::shared_ptr<GameCamera> camera);
  void IncrementTicks(float time_warp);
  void SetStartTime();
  void SetItemsInView(std::unordered_set<std::shared_ptr<GameObject>>* objects);
  void SetLevelEditorState(
      std::shared_ptr<LevelEditorState> level_editor_state);
  void SetPlayingState(PlayingState playing_state);

 private:
  std::shared_ptr<Level> level;
  std::shared_ptr<GameCamera> camera;
  std::shared_ptr<Player> player;
  std::shared_ptr<Sky> sky;
  std::unordered_map<std::string, std::shared_ptr<VideoTexture>> video_textures;
  std::unordered_set<std::shared_ptr<GameObject>>* objectsInView;
  std::shared_ptr<LevelEditorState> level_editor_state;
  GLFWwindow* window;
  std::shared_ptr<ParticleGenerator> particles;
  SoundEffects effects;

  double elapsed_ticks;
  uint64_t start_tick;      // value of elapsed_ticks when game started
  double start_time;        // value of glfwGetTime() when game started
  uint64_t music_end_tick;  // number of ticks we will be at when music ends
  PlayingState playing_state;
  uint64_t game_end_tick;  // tick when the game was won or lost
  double game_end_time;    // value of glfwGetTime() when game was won/lost
  bool previously_paused = false;
};

#endif
