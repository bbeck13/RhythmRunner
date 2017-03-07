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

#define COLLISION_WIDTH 0.420f

std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
GameUpdater::GetCollidingObjects(AxisAlignedBox primary_object,
                                 std::shared_ptr<Octree> tree) {
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>> collisions =
      std::make_shared<std::unordered_set<std::shared_ptr<GameObject>>>();
  std::queue<Node*> toVisit;
  toVisit.push(tree->GetRoot());

  while (!toVisit.empty()) {
    Node* node = toVisit.front();
    toVisit.pop();
    if (node->objects->empty()) {
      for (Node* child : *(node->children)) {
        if (AxisAlignedBox::IsColliding(child->boundingBox, primary_object)) {
          if (!(child->objects->empty() && child->children->empty())) {
            toVisit.push(child);
          }
        }
      }
    } else {
      for (std::shared_ptr<GameObject> objectInBox : *(node->objects)) {
        if (AxisAlignedBox::IsColliding(objectInBox->GetBoundingBox(),
                                        primary_object)) {
          collisions->insert(objectInBox);
        }
      }
    }
  }
  return collisions;
}

GameUpdater::GameUpdater() {}

GameUpdater::~GameUpdater() {}

void GameUpdater::Init(std::shared_ptr<GameState> game_state) {
  Reset(game_state);
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
  UpdatePlayer(game_state);
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
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  // reset the player
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
    music->setPlayingOffset(sf::Time::Zero);
    music->stop();
  }
}

void GameUpdater::UpdatePlayer(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();
  std::shared_ptr<Sky> sky = game_state->GetSky();

  // the max width to allow the player to be colliding with an object
  float collision_width = COLLISION_WIDTH;

  // Store player state before moving
  AxisAlignedBox previous_player_box = player->GetBoundingBox();
  float previous_player_velocity = player->GetYVelocity();

  // Check to see if the ground is no longer beneath the player,
  // in which case they should fall down
  if (player->GetGround()) {
    AxisAlignedBox ground_box = player->GetGround()->GetBoundingBox();
    // This will only consider when the player moves to the right of the
    // platform
    if (previous_player_box.GetMin().x > ground_box.GetMax().x) {
      // Ground is no longer beneath player
      player->RemoveGround();
      player->ChangeAnimation(Player::Animation::JUMPING,
                              game_state->GetElapsedTicks());
    }
  }

  // Jump if the user inputs a jump, or apply gravity.
  // Calculate velocity for moving ground objects.
  if (player->GetGround() && ImGui::GetIO().KeysDown[GLFW_KEY_SPACE]) {
    player->Jump(game_state->GetElapsedTicks());
    player->SetDoubleJump(true);
    player->RemoveGround();
    player->ChangeAnimation(Player::Animation::JUMPING,
                            game_state->GetElapsedTicks());
  } else if (player->GetGround()) {
    if (GameObject::Moves(player->GetGround()->GetSecondaryType())) {
      std::shared_ptr<MovingObject> moving =
          std::dynamic_pointer_cast<MovingObject>(player->GetGround());
      // Move player with moving object
      glm::vec3 movementVector = moving->GetMovementVector();
      player->SetYVelocity(movementVector.y * moving->GetVelocity().y);
      player->SetZVelocity(movementVector.z * moving->GetVelocity().z);
      // add to the allowed collision width
      collision_width += movementVector.y * moving->GetVelocity().y;
    } else if (player->GetGround()->GetSecondaryType() ==
               SecondaryType::DROPPING_PLATFORM) {
      std::shared_ptr<gameobject::DroppingPlatform> dropping =
          std::static_pointer_cast<gameobject::DroppingPlatform>(
              player->GetGround());
      // move the player with the dropping platform
      player->SetYVelocity(dropping->GetYVelocity());
    } else {
      // player is stuck to ground, make sure velocity is neutralized
      player->SetYVelocity(0.0f);
      player->SetZVelocity(0.0f);
    }
  } else {
    // player is in the air, apply gravity
    player->SetYVelocity(previous_player_velocity - PLAYER_GRAVITY);
    if (player->GetDoubleJump() &&
        InputBindings::KeyNewlyPressed(GLFW_KEY_SPACE)) {
      // Double jump
      player->Jump(game_state->GetElapsedTicks());
      player->SetDoubleJump(false);
    }
  }

  // always check for ducking (be ware of ducks)
  // let the player move up/down Z (sock it to me)
  if (ImGui::GetIO().KeysDown[GLFW_KEY_LEFT_SHIFT] ||
      ImGui::GetIO().KeysDown[GLFW_KEY_RIGHT_SHIFT]) {
    player->SetDucking(DuckDir::RIGHT);
  } else if (ImGui::GetIO().KeysDown[GLFW_KEY_LEFT] ||
             ImGui::GetIO().KeysDown[GLFW_KEY_H]) {
    player->MoveDownZ();
    player->SetDucking(DuckDir::LEFT);
  } else if (ImGui::GetIO().KeysDown[GLFW_KEY_RIGHT] ||
             ImGui::GetIO().KeysDown[GLFW_KEY_L]) {
    player->MoveUpZ();
    player->SetDucking(DuckDir::RIGHT);
  } else {
    player->SetDucking(DuckDir::NONE);
  }

  // Update player position based on new velocity.
  player->SetPosition(player->GetPosition() +
                      glm::vec3(DELTA_X_PER_TICK, player->GetYVelocity(),
                                player->GetZVelocity()));
  player->Animate(game_state->GetElapsedTicks());
  player->SnapToGround();  // aligns player with ground if grounded
  sky->SetPosition(sky->GetPosition() + glm::vec3(DELTA_X_PER_TICK, 0, 0));

  // Determine colliding objects.
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
      collidingObjects = GetCollidingObjects(player->GetBoundingBox(),
                                             game_state->GetLevel()->getTree());
  std::vector<std::shared_ptr<Obstacle>> colliding_obstacles;
  std::vector<std::shared_ptr<Collectible>> colliding_collectibles;
  for (std::shared_ptr<GameObject> game_object : *collidingObjects) {
    if (game_object->GetType() == ObjectType::COLLECTIBLE) {
      colliding_collectibles.push_back(
          std::static_pointer_cast<Collectible>(game_object));
    } else if (game_object->GetType() == ObjectType::OBSTACLE) {
      colliding_obstacles.push_back(
          std::static_pointer_cast<Obstacle>(game_object));
    }
  }

  // Collect the collectibles we are colliding with.
  for (std::shared_ptr<Collectible> collectible : colliding_collectibles) {
    if (!collectible->GetCollected()) {
      collectible->SetCollected();
      game_state->GetPlayer()->SetScore(game_state->GetPlayer()->GetScore() +
                                        1);
    }
  }

  // Apply collision logic for obstacles.
  if (!colliding_obstacles.empty()) {
    AxisAlignedBox player_box = player->GetBoundingBox();
    float player_min_y = previous_player_box.GetMin().y;
    // Handle the collisions
    for (std::shared_ptr<GameObject> colliding_object : colliding_obstacles) {
      // Are we in the air, and is the object we are colliding with below us?
      // If so, then set it as our current ground
      AxisAlignedBox colliding_box = colliding_object->GetBoundingBox();
      float colliding_max_y = colliding_object->GetBoundingBox().GetMax().y;

      // If the player's bounding box was "above" (within 1 velocity unit +
      // allowed COLLISION_WIDTH error) the platform's box before
      // collision,
      // then we consider the collision "landing" on the platform
      if (std::abs(player_min_y - colliding_max_y) <
          (std::abs(player->GetYVelocity()) + collision_width)) {
        // above colliding box, we are grounded on this platform
        // now attach player to the ground
        player->SetGround(colliding_object);
        player->SetYVelocity(0);
        player->SetZVelocity(0);
        player->SnapToGround();
        player->ChangeAnimation(Player::Animation::GROUNDED,
                                game_state->GetElapsedTicks());
        player_min_y = player->GetBoundingBox().GetMin().y;
        // If the ground is now a dropping platform drop it
        if (player->GetGround()->GetSecondaryType() ==
            SecondaryType::DROPPING_PLATFORM) {
          std::shared_ptr<gameobject::DroppingPlatform> dropping =
              std::dynamic_pointer_cast<gameobject::DroppingPlatform>(
                  player->GetGround());
          dropping->SetDropping();
        }
      } else {
        // The collision was not from above, so reset the game
        game_state->SetPlayingState(GameState::PlayingState::FAILURE);
        return;
      }
    }
  }

  // Check to see if the player fell out of the world.
  if (previous_player_box.GetMin().y <
      game_state->GetLevel()->getTree()->GetKillZone()) {
    game_state->SetPlayingState(GameState::PlayingState::FAILURE);
  }
}

void GameUpdater::UpdateCamera(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();

  glm::vec3 player_position = game_state->GetPlayer()->GetPosition();
  glm::vec3 camera_spacing = camera->GetCameraPlayerSpacing();
  float minCameraYLimit = player_position[1] + 1.5;
  float maxCameraYLimit = player_position[1] + 3.5;

  if (ImGui::GetIO().KeysDown[GLFW_KEY_D]) {
    camera->revolveAroundLookAt(0.0f, CAMERA_YAW_MOVE/1000.0f);
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_A]) {
    camera->revolveAroundLookAt(0.0f, -1*CAMERA_YAW_MOVE/1000.0f);
  }
  if (game_state->GetPlayer()->GetGround()) {
    if (ImGui::GetIO().KeysDown[GLFW_KEY_W]) {
      glm::vec3 prevCameraPos = camera->getPosition();
      camera->revolveAroundLookAt(CAMERA_YAW_MOVE/1000.0f, 0.0f);
      if (camera->getPosition()[1] > maxCameraYLimit) {
        camera->setPosition(prevCameraPos);
      }
    }
    if (ImGui::GetIO().KeysDown[GLFW_KEY_S]) {
      glm::vec3 prevCameraPos = camera->getPosition();
      camera->revolveAroundLookAt(-1*CAMERA_YAW_MOVE/1000.0f, 0.0f);
      if (camera->getPosition()[1] < minCameraYLimit) {
        camera->setPosition(prevCameraPos);
      }
    }
  }

  camera->setPosition(camera->getPosition() + glm::vec3(DELTA_X_PER_TICK, 0, 0));

  // Always look directly at the player.
  // Add FORWARD_CAMERA_SPACING to align camera
  camera->setLookAt(player_position + glm::vec3(FORWARD_CAMERA_SPACING, 0, 0));

  if (ImGui::GetIO().KeysDown[GLFW_KEY_1]) {  // view 1
    camera->SetCameraPlayerSpacing(
        glm::vec3(FORWARD_CAMERA_SPACING, CAMERA_Y_SPACING, CAMERA_Z_SPACING));
  } else if (ImGui::GetIO().KeysDown[GLFW_KEY_2]) {  // views from the 6
    // 2.5d my ass Zoe!
    camera->SetCameraPlayerSpacing(
        glm::vec3(CAMERA_VIEW_1_X, CAMERA_VIEW_1_Y, CAMERA_VIEW_1_Z));
  }
  glm::vec3 newCameraPos = camera->getPosition();
  if (newCameraPos[1] < minCameraYLimit) {
    camera->setPosition(glm::vec3(newCameraPos[0],
                                  minCameraYLimit,
                                  newCameraPos[2]));
  } else if (newCameraPos[1] > maxCameraYLimit) {
    camera->setPosition(glm::vec3(newCameraPos[0],
                                  maxCameraYLimit,
                                  newCameraPos[2]));
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
