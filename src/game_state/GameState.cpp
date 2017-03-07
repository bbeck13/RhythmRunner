// Joseph Arhar

#include "GameState.h"

#include "TimingConstants.h"

GameState::GameState(std::shared_ptr<Level> level,
                     std::shared_ptr<GameCamera> camera,
                     std::shared_ptr<Player> player,
                     std::shared_ptr<Sky> sky,
                     GLFWwindow* window)
    : level(level),
      camera(camera),
      player(player),
      sky(sky),
      window(window),
      elapsed_ticks(0),
      start_tick(0),
      start_time(0),
      playing_state(PlayingState::PLAYING),
      game_end_tick(0),
      game_end_time(0) {
  objectsInView = new std::unordered_set<std::shared_ptr<GameObject>>();
  this->music_end_tick =
      level->getMusic()->getDuration().asMicroseconds() * TICKS_PER_MICRO +
      GetMusicStartTick();
}

GameState::~GameState() {
  delete objectsInView;
}

GLFWwindow* GameState::GetWindow() {
  return this->window;
}

std::shared_ptr<Level> GameState::GetLevel() {
  return this->level;
}

std::shared_ptr<GameCamera> GameState::GetCamera() {
  return this->camera;
}

std::shared_ptr<Player> GameState::GetPlayer() {
  return this->player;
}

std::shared_ptr<Sky> GameState::GetSky() {
  return this->sky;
}

std::unordered_map<std::string, std::shared_ptr<VideoTexture>>
GameState::GetVideoTextures() {
  return this->video_textures;
}

bool GameState::ReachedEndOfLevel() {
  return elapsed_ticks > music_end_tick;
}

uint64_t GameState::GetElapsedTicks() {
  return elapsed_ticks;
}

uint64_t GameState::GetStartTick() {
  return start_tick;
}

uint64_t GameState::GetMusicStartTick() {
  // the music start tick is always dependent on the start tick
  return start_tick + PREGAME_SECONDS * TICKS_PER_SECOND;
}

double GameState::GetStartTime() {
  return start_time;
}

GameState::PlayingState GameState::GetPlayingState() {
  return playing_state;
}

uint64_t GameState::GetEndingTick() {
  return game_end_tick;
}

double GameState::GetEndingTime() {
  return game_end_time;
}

double GameState::GetProgressRatio() {
  return elapsed_ticks / (double)music_end_tick;
}

void GameState::AddVideoTexture(std::string name,
                                std::shared_ptr<VideoTexture> texture) {
  this->video_textures[name] = texture;
}

void GameState::SetLevel(std::shared_ptr<Level> level) {
  this->level = level;
}

void GameState::SetCamera(std::shared_ptr<GameCamera> camera) {
  this->camera = camera;
}

void GameState::IncrementTicks() {
  elapsed_ticks++;
}

void GameState::SetStartTime() {
  elapsed_ticks = 0;
  start_tick = elapsed_ticks;
  start_time = glfwGetTime();
}

void GameState::SetItemsInView(
    std::unordered_set<std::shared_ptr<GameObject>>* objects) {
  delete objectsInView;
  this->objectsInView = objects;
}

std::unordered_set<std::shared_ptr<GameObject>>* GameState::GetObjectsInView() {
  return objectsInView;
}

std::shared_ptr<LevelEditorState> GameState::GetLevelEditorState() {
  return level_editor_state;
}

void GameState::SetLevelEditorState(
    std::shared_ptr<LevelEditorState> level_editor_state) {
  this->level_editor_state = level_editor_state;
}

void GameState::SetPlayingState(PlayingState playing_state) {
  this->playing_state = playing_state;
  game_end_tick = elapsed_ticks;
  game_end_time = glfwGetTime();

  // update music
  std::shared_ptr<sf::Music> music = GetLevel()->getMusic();
  switch (playing_state) {
    case GameState::PlayingState::PLAYING:
      // on resume start the music back up
      if (music->getStatus() != sf::SoundSource::Status::Playing) {
        music->play();
      }
      break;
    case GameState::PlayingState::PAUSED:
      // pause the music
      if (music->getStatus() == sf::SoundSource::Status::Playing) {
        music->pause();
      }
      break;
    case GameState::PlayingState::FAILURE:
      if (music->getStatus() == sf::SoundSource::Status::Playing) {
        music->stop();
      }
      player->ChangeAnimation(Player::Animation::FAILURE, elapsed_ticks);
      break;
    case GameState::PlayingState::SUCCESS:
      player->ChangeAnimation(Player::Animation::SUCCESS, elapsed_ticks);

      // stop the music
      if (music->getStatus() == sf::SoundSource::Status::Playing) {
        music->stop();
        music->setPlayingOffset(sf::Time::Zero);
      }
      break;
  }
}
