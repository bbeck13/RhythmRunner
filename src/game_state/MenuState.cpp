// Joseph Arhar

#include "MenuState.h"

MenuState::MenuState() {}

MenuState::~MenuState() {}

std::string MenuState::GetMusicPath() {
  return music_path;
}

void MenuState::SetMusicPath(std::string music_path) {
  this->music_path = music_path;
}
