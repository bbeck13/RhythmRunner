// Joseph Arhar

#ifndef MENU_STATE_H_
#define MENU_STATE_H_

#include <string>

class MenuState {
 public:
  MenuState();
  ~MenuState();

  std::string GetMusicPath();
  std::string GetLevelPath();

  void SetMusicPath(std::string music_path);

  void SetLevelPath(std::string level_path);

 private:
  std::string music_path;
  std::string level_path;
};

#endif
