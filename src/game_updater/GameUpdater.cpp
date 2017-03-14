// Joseph Arhar
// Jotaro Kujo

#include "GameUpdater.h"

#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <iostream>
#include <queue>

#include "DroppingPlatform.h"
#include "InputBindings.h"
#include "Logging.h"
#include "MovingObject.h"
#include "Octree.h"
#include "TimingConstants.h"
#include "VideoTexture.h"
#include "Particles.h"

#define CAMERA_SPACING_INCREMENT 0.01f

GameUpdater::GameUpdater() {}

GameUpdater::~GameUpdater() {}

void GameUpdater::Init(std::shared_ptr<GameState> game_state) {
  Reset(game_state);
}

void GameUpdater::PostGameUpdate(std::shared_ptr<GameState> game_state) {
  // run animations after winning or losing
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (obj->GetType() == ObjectType::COLLECTIBLE) {
      std::shared_ptr<Collectible> collectible =
          std::dynamic_pointer_cast<Collectible>(obj);
      collectible->Animate(game_state->GetPlayer()->GetTimeWarp());
      if (collectible->GetCollected()) {
        collectible->IncrementTicksCollected(
            game_state->GetPlayer()->GetTimeWarp());
      }
    }
  }
  player_updater.AnimatePlayer(game_state);
}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  if (game_state->ReachedEndOfLevel()) {
    game_state->SetPlayingState(GameState::PlayingState::SUCCESS);
    game_state->IncrementTicks(game_state->GetPlayer()->GetTimeWarp());
    return;
  }

  if (InputBindings::KeyPressed(GLFW_KEY_ESCAPE)) {
    game_state->SetPlayingState(GameState::PlayingState::PAUSED);
    return;
  }

  // check to see if the music should start on this tick
  std::shared_ptr<sf::Music> music = game_state->GetLevel()->getMusic();
  if (game_state->GetMusicStartTick() == game_state->GetElapsedTicks()) {
    music->play();
    music->setLoop(false);
  }

  UpdateLevel(game_state);
  player_updater.MovePlayer(game_state);
  player_updater.AnimatePlayer(game_state);
  player_updater.CollisionCheck(game_state);
  player_updater.PowerUpPlayer(game_state);
  UpdateCamera(game_state, true);

  game_state->IncrementTicks(game_state->GetPlayer()->GetTimeWarp());
}

void GameUpdater::UpdateLevel(std::shared_ptr<GameState> game_state) {
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    // Moving the moving objects
    if (GameObject::Moves(obj->GetSecondaryType())) {
      std::shared_ptr<MovingObject> movingObj =
          std::dynamic_pointer_cast<MovingObject>(obj);
      obj->SetPosition(movingObj->updatePosition(
          obj->GetPosition(), game_state->GetPlayer()->GetTimeWarp()));

      // Drop the dropping Platforms
    } else if (obj->GetSecondaryType() == SecondaryType::DROPPING_PLATFORM_UP ||
               obj->GetSecondaryType() ==
                   SecondaryType::DROPPING_PLATFORM_DOWN) {
      std::shared_ptr<gameobject::DroppingPlatform> dropper =
          std::dynamic_pointer_cast<gameobject::DroppingPlatform>(obj);
      if (dropper->IsDropping()) {
        obj->SetPosition(obj->GetPosition() +
                         glm::vec3(0.0f,
                                   dropper->GetYVelocity() *
                                       game_state->GetPlayer()->GetTimeWarp(),
                                   0.0f));
      }
    } else if (obj->GetType() == ObjectType::COLLECTIBLE) {
      std::shared_ptr<Collectible> collectible =
          std::dynamic_pointer_cast<Collectible>(obj);
      collectible->Animate(game_state->GetPlayer()->GetTimeWarp());
      if (collectible->GetCollected()) {
        collectible->IncrementTicksCollected(
            game_state->GetPlayer()->GetTimeWarp());
      }
    }
  }

  game_state->GetSky()->SetPosition(
      game_state->GetSky()->GetPosition() +
      glm::vec3(game_state->GetPlayer()->GetXVelocity(),
                game_state->GetPlayer()->GetYVelocity(),
                game_state->GetPlayer()->GetZVelocity()));
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  // reset the player
  player_updater.ChangeAnimation(game_state, Player::Animation::JUMPING);
  game_state->SetPlayingState(GameState::PlayingState::PLAYING);
  game_state->GetSky()->SetPosition(glm::vec3(0, 0, -10));
  game_state->GetVideoTextures()["sky"]->ResetFrameCount();

  // TODO - iterate over all video textures and reset their playback
  // reset collectibles and moving objects
  for (std::shared_ptr<GameObject> obj :
       *game_state->GetLevel()->getObjects()) {
    if (obj->GetType() == ObjectType::COLLECTIBLE) {
      std::shared_ptr<Collectible> c =
          std::static_pointer_cast<Collectible>(obj);
      c->SetUncollected();
    }
    // reset the moving objects
    if (obj->GetSecondaryType() == SecondaryType::MOVING_PLATFORM) {
      std::shared_ptr<MovingObject> movingObj =
          std::dynamic_pointer_cast<MovingObject>(obj);
      movingObj->Reset();
      obj->SetPosition(movingObj->GetOriginalPosition());
      // reset the dropping platforms
    } else if (obj->GetSecondaryType() == SecondaryType::DROPPING_PLATFORM_UP ||
               obj->GetSecondaryType() ==
                   SecondaryType::DROPPING_PLATFORM_DOWN) {
      std::shared_ptr<gameobject::DroppingPlatform> dropping =
          std::dynamic_pointer_cast<gameobject::DroppingPlatform>(obj);
      dropping->Reset();
    }
  }

  // reset timing
  game_state->SetStartTime();
  game_state->GetPlayer()->Reset();
  game_state->GetCamera()->Reset();

  // stop the music if it is playing
  std::shared_ptr<sf::Music> music = game_state->GetLevel()->getMusic();
  if (music->getStatus() != sf::SoundSource::Status::Stopped) {
    music->stop();
  }
  music->setPitch(1);
  music->setVolume(50);
}

void GameUpdater::UpdateCamera(std::shared_ptr<GameState> game_state) {
  UpdateCamera(game_state, false);
}

void GameUpdater::UpdateCamera(std::shared_ptr<GameState> game_state,
                               bool update_with_player) {
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  glm::vec3 player_position = game_state->GetPlayer()->GetPosition();
  float minCameraYLimit = player_position[1] + 1.5;
  float maxCameraYLimit = player_position[1] + 3.5;

  // update camera with cursor
  std::pair<double, double> cursor_diff = InputBindings::GetCursorDiff();
  camera->revolveAroundLookAt(0.0f,
                              -cursor_diff.first * CAMERA_YAW_MOVE / 4000.0f);

  // update camera with keys
  if (InputBindings::KeyDown(GLFW_KEY_L) ||
      InputBindings::KeyDown(GLFW_KEY_RIGHT)) {
    camera->revolveAroundLookAt(0.0f, CAMERA_YAW_MOVE / 1000.0f);
  }
  if (InputBindings::KeyDown(GLFW_KEY_H) ||
      InputBindings::KeyDown(GLFW_KEY_LEFT)) {
    camera->revolveAroundLookAt(0.0f, -CAMERA_YAW_MOVE / 1000.0f);
  }
  if (InputBindings::KeyDown(GLFW_KEY_K) ||
      InputBindings::KeyDown(GLFW_KEY_UP)) {
    glm::vec3 prevCameraPos = camera->getPosition();
    camera->revolveAroundLookAt(CAMERA_YAW_MOVE / 1000.0f, 0.0f);
    if (camera->getPosition()[1] > maxCameraYLimit) {
      camera->setPosition(prevCameraPos);
    }
  }
  if (InputBindings::KeyDown(GLFW_KEY_J) ||
      InputBindings::KeyDown(GLFW_KEY_DOWN)) {
    glm::vec3 prevCameraPos = camera->getPosition();
    camera->revolveAroundLookAt(-1 * CAMERA_YAW_MOVE / 1000.0f, 0.0f);
    if (camera->getPosition()[1] < minCameraYLimit) {
      camera->setPosition(prevCameraPos);
    }
  }
  glm::vec3 camera_position = camera->getPosition();
  glm::vec3 look_at = camera->getLookAt();
  glm::vec3 view = camera_position - look_at;
  if (InputBindings::KeyDown(GLFW_KEY_C)) {
    camera->setPosition(camera->getPosition() -
                        CAMERA_SPACING_INCREMENT * view);
  }
  if (InputBindings::KeyDown(GLFW_KEY_E)) {
    camera->setPosition(camera->getPosition() +
                        CAMERA_SPACING_INCREMENT * view);
  }

  if (update_with_player) {
    camera->setPosition(camera->getPosition() +
                        glm::vec3(game_state->GetPlayer()->GetXVelocity(), 0,
                                  game_state->GetPlayer()->GetZVelocity()));
  }

  // Always look directly at the player.
  // Add FORWARD_CAMERA_SPACING to align camera
  camera->setLookAt(player_position +
                    glm::vec3(camera->GetForwardSpacing(), 0, 0));

  if (InputBindings::KeyDown(GLFW_KEY_1)) {  // view 1
    camera->SetCameraPlayerSpacing(
        glm::vec3(FORWARD_CAMERA_SPACING, CAMERA_Y_SPACING, CAMERA_Z_SPACING));
    glm::vec3 camera_spacing = camera->GetCameraPlayerSpacing();
    camera->setPosition(player_position + camera_spacing);
  } else if (InputBindings::KeyDown(GLFW_KEY_2)) {  // views from the 6
    // 2.5d my ass Zoe!
    camera->SetCameraPlayerSpacing(
        glm::vec3(CAMERA_VIEW_1_X, CAMERA_VIEW_1_Y, CAMERA_VIEW_1_Z));
    glm::vec3 camera_spacing = camera->GetCameraPlayerSpacing();
    camera->setPosition(player_position + camera_spacing);
  }
  glm::vec3 newCameraPos = camera->getPosition();
  if (newCameraPos[1] < minCameraYLimit) {
    camera->setPosition(
        glm::vec3(newCameraPos[0], minCameraYLimit, newCameraPos[2]));
  } else if (newCameraPos[1] > maxCameraYLimit) {
    camera->setPosition(
        glm::vec3(newCameraPos[0], maxCameraYLimit, newCameraPos[2]));
  }
}

uint64_t GameUpdater::CalculateTargetTicks(
    std::shared_ptr<GameState> game_state) {
  if (game_state->GetPlayingState() != GameState::PlayingState::PLAYING) {
    // don't tick anymore if the game is over/paused
    return game_state->GetElapsedTicks();
  }

#ifdef DEBUG  // Step-by-step mode for debugging
  static bool step_mode = false;
  if (ImGui::GetIO().KeyCtrl && InputBindings::KeyDown(GLFW_KEY_K)) {
    step_mode = true;
  }
  if (step_mode) {
    if (InputBindings::KeyPressed(GLFW_KEY_K)) {
      return game_state->GetElapsedTicks() + 1;
    } else {
      return game_state->GetElapsedTicks();
    }
  }
#endif

  if (game_state->GetLevel()->getMusic()->getStatus() ==
      sf::Music::Status::Playing) {
    // get time elapsed in music
    // then, calculate target ticks elapsed based on music time elapsed
    int64_t music_offset_micros =
        game_state->GetLevel()->getMusic()->getPlayingOffset().asMicroseconds();
    return music_offset_micros * TICKS_PER_MICRO + game_state->GetStartTick() +
           game_state->GetMusicStartTick();
  } else {
    double elapsed_seconds = glfwGetTime() - game_state->GetStartTime();
    return elapsed_seconds * TICKS_PER_SECOND;
  }
}
