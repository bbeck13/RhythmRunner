#ifndef SOUND_EFFECTS_H_
#define SOUND_EFFECTS_H_

#include <SFML/Audio.hpp>
#include <memory>
#include <string>

class SoundEffects {
 public:
  SoundEffects();
  ~SoundEffects();

  void OhYes();
  void Pause();
  void Unpause();
  void Death();
  bool ComingBackFromAPause();

 private:
  static const std::string oh_yes_file;
  static const std::string pause_file;
  static const std::string unpause_file;
  static const std::string death1_file;
  static const std::string death2_file;
  static const std::string death3_file;
  static const std::string death4_file;

  std::shared_ptr<sf::Music> oh_yes;
  std::shared_ptr<sf::Music> pause;
  std::shared_ptr<sf::Music> unpause;
  std::vector<std::shared_ptr<sf::Music>> deaths;
};
#endif
