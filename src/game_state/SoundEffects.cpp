#include "SoundEffects.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

const std::string SoundEffects::oh_yes_file(ASSET_DIR "/sounds/oh_yes.wav");
const std::string SoundEffects::pause_file(ASSET_DIR "/sounds/pause.wav");
const std::string SoundEffects::unpause_file(ASSET_DIR "/sounds/unpause.wav");
const std::string SoundEffects::death1_file(ASSET_DIR "/sounds/death1.wav");
const std::string SoundEffects::death2_file(ASSET_DIR "/sounds/death2.wav");
const std::string SoundEffects::death3_file(ASSET_DIR "/sounds/death3.wav");
const std::string SoundEffects::death4_file(ASSET_DIR "/sounds/death4.wav");

SoundEffects::SoundEffects() {
  this->oh_yes = std::make_shared<sf::Music>();
  if (!oh_yes->openFromFile(oh_yes_file)) {
    std::cerr << "Couldn't load " << oh_yes_file << std::endl;
    exit(EXIT_FAILURE);
  }
  this->pause = std::make_shared<sf::Music>();
  if (!pause->openFromFile(pause_file)) {
    std::cerr << "Couldn't load " << pause_file << std::endl;
    exit(EXIT_FAILURE);
  }
  this->unpause = std::make_shared<sf::Music>();
  if (!unpause->openFromFile(unpause_file)) {
    std::cerr << "Couldn't load " << unpause_file << std::endl;
    exit(EXIT_FAILURE);
  }
  std::shared_ptr<sf::Music> death;
  death = std::make_shared<sf::Music>();
  if (!death->openFromFile(death1_file)) {
    std::cerr << "Couldn't load " << death1_file << std::endl;
    exit(EXIT_FAILURE);
  }
  deaths.push_back(death);

  death = std::make_shared<sf::Music>();
  if (!death->openFromFile(death2_file)) {
    std::cerr << "Couldn't load " << death2_file << std::endl;
    exit(EXIT_FAILURE);
  }
  deaths.push_back(death);

  death = std::make_shared<sf::Music>();
  if (!death->openFromFile(death3_file)) {
    std::cerr << "Couldn't load " << death3_file << std::endl;
    exit(EXIT_FAILURE);
  }
  deaths.push_back(death);

  death = std::make_shared<sf::Music>();
  if (!death->openFromFile(death4_file)) {
    std::cerr << "Couldn't load " << death4_file << std::endl;
    exit(EXIT_FAILURE);
  }

  deaths.push_back(death);
}

SoundEffects::~SoundEffects() {}

void SoundEffects::OhYes() {
  if (oh_yes->getStatus() != sf::SoundSource::Playing) {
    oh_yes->play();
    oh_yes->setLoop(false);
  }
}

void SoundEffects::Pause() {
  if (pause->getStatus() != sf::SoundSource::Playing) {
    pause->play();
    pause->setLoop(false);
  }
}

void SoundEffects::Unpause() {
  if (unpause->getStatus() != sf::SoundSource::Playing) {
    unpause->play();
    unpause->setLoop(false);
  }
}

void SoundEffects::Death() {
  std::srand(std::time(NULL));
  int index = std::rand() % deaths.size();
  deaths.at(index)->play();
  deaths.at(index)->setLoop(false);
}

bool SoundEffects::ComingBackFromAPause() {
  return unpause->getStatus() != sf::SoundSource::Playing;
}
