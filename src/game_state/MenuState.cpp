// Joseph Arhar

#include "MenuState.h"

MenuState::MenuState() {}

MenuState::~MenuState() {}

std::string MenuState::GetMusicPath() {
  return music_path;
}

MenuMode MenuState::GetMenuMode() {
  return menu_mode;
}

void MenuState::SetMusicPath(std::string music_path) {
  this->music_path = music_path;
}

void MenuState::SetMenuMode(MenuMode menu_mode) {
  this->menu_mode = menu_mode;
}
