// Joseph Arhar

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <memory>
#include <vector>

#include "GameObject.h"
#include "Level.h"
#include "GameCamera.h"
#include "Player.h"

class GameState {
 public:
  GameState(std::shared_ptr<Level> level,
            std::shared_ptr<GameCamera> camera,
            std::shared_ptr<Player> player);
  ~GameState();

  std::shared_ptr<Level> GetLevel();
  std::shared_ptr<GameCamera> GetCamera();
  std::shared_ptr<Player> GetPlayer();
  bool Done();
  uint64_t GetElapsedTicks();
  uint64_t GetTimingStartTick();
  bool GetMusicTimingMode();

  void SetLevel(std::shared_ptr<Level> level);
  void SetCamera(std::shared_ptr<GameCamera> camera);
  void SetDone(bool done);
  void IncrementTicks();
  void SetTimingStartTick();
  void SetMusicTimingMode(bool music_timing_mode);

 private:
  std::shared_ptr<Level> level;
  std::shared_ptr<GameCamera> camera;
  std::shared_ptr<Player> player;
  bool done;
  uint64_t elapsed_ticks;
  uint64_t timing_start_tick;
  bool music_timing_mode;
};

#endif
