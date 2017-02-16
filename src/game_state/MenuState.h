// Joseph Arhar

#ifndef MENU_STATE_H_
#define MENU_STATE_H_

#include <string>

class MenuState {
 public:
  MenuState();
  ~MenuState();

  std::string GetMusicPath();

  void SetMusicPath(std::string music_path);

 private:
  std::string music_path;
};

#endif
