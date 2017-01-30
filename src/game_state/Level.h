// bnbeck
#ifndef LEVEL_H_
#define LEVEL_H_

#include <memory>
#include <SFML/Audio.hpp>

#include "GameObject.h"
#include "Platform.h"

class Level {
 public:
  Level(std::shared_ptr<sf::Music> music,
        std::shared_ptr<std::vector<Platform>> level);
  ~Level();

  /* get music returns the music to be controlled, every 1 platform corresponds
   to 500 ms of music*/
  std::shared_ptr<sf::Music> getMusic();
  /* get the platforms that make up this level, this vector shall be in order
   such that at 500 ms in vector.get(1) (the second platform) should be in the
   foreground */
  std::shared_ptr<std::vector<Platform>> getLevel();

 private:
  std::shared_ptr<sf::Music> music;
  std::shared_ptr<std::vector<Platform>> level;
};

#endif
