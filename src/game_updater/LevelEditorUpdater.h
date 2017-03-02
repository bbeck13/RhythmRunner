
#ifndef LEVEL_EDITOR_UPDATER_H_
#define LEVEL_EDITOR_UPDATER_H_

#include <memory>

#include "ProgramMode.h"
#include "GameState.h"
#include "GameObject.h"

class LevelEditorUpdater {
 public:
  LevelEditorUpdater();
  ~LevelEditorUpdater();

  LevelProgramMode Update(std::shared_ptr<GameState> game_state);
  void Init(std::shared_ptr<GameState> game_state);

 private:
  void UpdateLevel(std::shared_ptr<GameState> game_state);
  void UpdateCamera(std::shared_ptr<GameState> game_state);
};

#endif
