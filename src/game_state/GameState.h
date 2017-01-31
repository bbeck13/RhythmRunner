// Joseph Arhar

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <memory>
#include <vector>

#include "GameObject.h"
#include "Level.h"
#include "GameCamera.h"
#include "Player.h"
#include "Note.h"

class GameState {
 public:
  GameState(std::shared_ptr<Level> level,
            std::shared_ptr<Note> notes,
            std::shared_ptr<GameCamera> camera,
            std::shared_ptr<Player> player);
  ~GameState();

  std::shared_ptr<Level> GetLevel();
  std::shared_ptr<GameCamera> GetCamera();
  std::shared_ptr<Player> GetPlayer();

  void SetLevel(std::shared_ptr<Level> level);
  void SetCamera(std::shared_ptr<GameCamera> camera);
  void SetDone(bool done);
  bool Done();

 private:
  std::shared_ptr<Level> level;
  std::shared_ptr<Note> notes;
  std::shared_ptr<GameCamera> camera;
  std::shared_ptr<Player> player;
  bool done;
};

#endif
