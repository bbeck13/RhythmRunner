// bnbeck
#ifndef LEVEL_H_
#define LEVEL_H_

#include <memory>
#include <SFML/Audio.hpp>

#include "GameObject.h"
#include "Platform.h"
#include "Note.h"

class Level {
 public:
  Level(std::shared_ptr<sf::Music> music,
        std::shared_ptr<std::vector<std::shared_ptr<Platform>>> platforms,
        std::shared_ptr<std::vector<std::shared_ptr<Note>>> notes);
  ~Level();

  std::shared_ptr<sf::Music> getMusic();
  std::shared_ptr<std::vector<std::shared_ptr<Platform>>> getPlatforms();
  std::shared_ptr<std::vector<std::shared_ptr<Note>>> getNotes();

 private:
  std::shared_ptr<sf::Music> music;
  std::shared_ptr<std::vector<std::shared_ptr<Platform>>> platforms;
  std::shared_ptr<std::vector<std::shared_ptr<Note>>> notes;
};

#endif
