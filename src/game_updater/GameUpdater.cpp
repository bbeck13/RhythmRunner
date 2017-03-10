// Joseph Arhar

#include "GameUpdater.h"

// imgui and glfw are needed for keyboard input
#include <imgui.h>
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

GameUpdater::GameUpdater() {}

GameUpdater::~GameUpdater() {}

void GameUpdater::Init(std::shared_ptr<GameState> game_state) {
  Reset(game_state);
}

void GameUpdater::PostGameUpdate(std::shared_ptr<GameState> game_state) {
  // animate the player after you win or lose
  player_updater.AnimatePlayer(game_state);
  game_state->IncrementTicks(); // increment ticks just for animations
}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  if (game_state->ReachedEndOfLevel()) {
    game_state->SetPlayingState(GameState::PlayingState::SUCCESS);
    game_state->IncrementTicks();
    return;
  }

  if (InputBindings::KeyNewlyPressed(GLFW_KEY_ESCAPE)) {
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
  UpdateCamera(game_state);

  game_state->IncrementTicks();
}

void GameUpdater::UpdateLevel(std::shared_ptr<GameState> game_state) {
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    // Moving the moving objects
    if (GameObject::Moves(obj->GetSecondaryType())) {
      std::shared_ptr<MovingObject> movingObj =
          std::dynamic_pointer_cast<MovingObject>(obj);
      obj->SetPosition(movingObj->updatePosition(obj->GetPosition()));

      // Drop the dropping Platforms
    } else if (obj->GetSecondaryType() == SecondaryType::DROPPING_PLATFORM) {
      std::shared_ptr<gameobject::DroppingPlatform> dropper =
          std::dynamic_pointer_cast<gameobject::DroppingPlatform>(obj);
      if (dropper->IsDropping()) {
        obj->SetPosition(obj->GetPosition() +
                         glm::vec3(0.0f, dropper->GetYVelocity(), 0.0f));
      }
    } else if (obj->GetSecondaryType() == SecondaryType::NOTE) {
      std::shared_ptr<gameobject::Note> note =
          std::dynamic_pointer_cast<gameobject::Note>(obj);
      note->Animate();
    }
  }

  game_state->GetSky()->SetPosition(game_state->GetSky()->GetPosition() +
                                    glm::vec3(DELTA_X_PER_TICK, 0, 0));
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
    } else if (obj->GetSecondaryType() == SecondaryType::DROPPING_PLATFORM) {
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
}

void GameUpdater::UpdateCamera(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();

  glm::vec3 player_position = game_state->GetPlayer()->GetPosition();
  glm::vec3 camera_spacing = camera->GetCameraPlayerSpacing();
  float minCameraYLimit = player_position[1] + 1.5;
  float maxCameraYLimit = player_position[1] + 3.5;

  if (ImGui::GetIO().KeysDown[GLFW_KEY_D]) {
    camera->revolveAroundLookAt(0.0f, CAMERA_YAW_MOVE / 1000.0f);
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_A]) {
    camera->revolveAroundLookAt(0.0f, -1 * CAMERA_YAW_MOVE / 1000.0f);
  }
  if (game_state->GetPlayer()->GetGround()) {
    if (ImGui::GetIO().KeysDown[GLFW_KEY_W]) {
      glm::vec3 prevCameraPos = camera->getPosition();
      camera->revolveAroundLookAt(CAMERA_YAW_MOVE / 1000.0f, 0.0f);
      if (camera->getPosition()[1] > maxCameraYLimit) {
        camera->setPosition(prevCameraPos);
      }
    }
    if (ImGui::GetIO().KeysDown[GLFW_KEY_S]) {
      glm::vec3 prevCameraPos = camera->getPosition();
      camera->revolveAroundLookAt(-1 * CAMERA_YAW_MOVE / 1000.0f, 0.0f);
      if (camera->getPosition()[1] < minCameraYLimit) {
        camera->setPosition(prevCameraPos);
      }
    }
  }

  camera->setPosition(camera->getPosition() +
                      glm::vec3(DELTA_X_PER_TICK, 0, 0));

  // Always look directly at the player.
  // Add FORWARD_CAMERA_SPACING to align camera
  camera->setLookAt(player_position + glm::vec3(FORWARD_CAMERA_SPACING, 0, 0));

  if (ImGui::GetIO().KeysDown[GLFW_KEY_1]) {  // view 1
    camera->SetCameraPlayerSpacing(
        glm::vec3(FORWARD_CAMERA_SPACING, CAMERA_Y_SPACING, CAMERA_Z_SPACING));
    glm::vec3 camera_spacing = camera->GetCameraPlayerSpacing();
    camera->setPosition(player_position + camera_spacing);
  } else if (ImGui::GetIO().KeysDown[GLFW_KEY_2]) {  // views from the 6
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
  if (ImGui::GetIO().KeyShift && ImGui::GetIO().KeysDown[GLFW_KEY_L]) {
    step_mode = true;
  }
  if (step_mode) {
    if (InputBindings::KeyNewlyPressed(GLFW_KEY_K)) {
      return game_state->GetElapsedTicks() + 1;
    } else {
      InputBindings::StoreKeypresses();
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
