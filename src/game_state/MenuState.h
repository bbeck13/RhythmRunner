// Joseph Arhar

#ifndef MENU_STATE_H_
#define MENU_STATE_H_

#include <string>

// This is used to signal when the game should start
// TODO(jarhar): is there a better way to do this?
enum class MenuMode {
  WAITING_FOR_INPUT, START_GAME
};

class MenuState {
 public:
  MenuState();
  ~MenuState();

  std::string GetMusicPath();
  MenuMode GetMenuMode();

  void SetMusicPath(std::string music_path);
  void SetMenuMode(MenuMode menu_mode);

 private:
  std::string music_path;
  MenuMode menu_mode;
};

#endif
