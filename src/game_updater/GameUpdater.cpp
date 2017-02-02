// Joseph Arhar

#include "game_updater/GameUpdater.h"

#include <glm/ext.hpp>
#include <iostream>

#include "helpers/Logging.h"
#include "helpers/TimingConstants.h"

#define DISTANCE_BELOW_CAMERA 1.8
#define DISTANCE_BEHIND_CAMERA 3
#define DELTA_Y 0.007f
#define MIN_DELTA_X 0.055f
namespace {

template <typename T>
std::vector<std::shared_ptr<T>> GetCollidingObjects(
    AxisAlignedBox primary_object,
    std::shared_ptr<std::vector<std::shared_ptr<T>>> secondary_objects) {
  std::vector<std::shared_ptr<T>> colliding_objects;
  // TODO(jarhar): make this more efficient by culling secondary objects
  for (std::shared_ptr<T> secondary_object : *secondary_objects) {
    if (AxisAlignedBox::IsColliding(primary_object,
                                    secondary_object->GetBoundingBox())) {
      colliding_objects.push_back(secondary_object);
    }
  }

  return colliding_objects;
}
}

GameUpdater::GameUpdater() {}

GameUpdater::~GameUpdater() {}

void GameUpdater::Init(std::shared_ptr<GameState> game_state) {
  Reset(game_state);
}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  // check to see if the music ended, in which case the level is complete
  if (game_state->GetLevel()->getMusic()->getStatus() ==
      sf::Music::Status::Stopped) {
    game_state->SetDone(true);
  }

  UpdatePlayer(game_state);
  UpdateCamera(game_state);

  game_state->IncrementTicks();
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  // set the music back to the beginning of the song
  std::shared_ptr<sf::Music> music = game_state->GetLevel()->getMusic();
  if (music->getStatus() == sf::SoundSource::Status::Playing) {
    music->setPlayingOffset(sf::Time::Zero);
  } else if (music->getStatus() == sf::SoundSource::Status::Paused) {
    music->setPlayingOffset(sf::Time::Zero);
    music->play();
  } else {
    music->play();
    music->setLoop(false);
  }

  // reset the player
  game_state->SetDone(false);
  game_state->GetPlayer()->SetPosition(Player::INITIAL_POSITION);
  game_state->GetPlayer()->SetScore(0);

  // reset collectible notes
  for (std::shared_ptr<Note> note : *game_state->GetLevel()->getNotes()) {
    note->SetUncollected();
  }

  // reset timing
  game_state->SetMusicTimingMode(true);
  game_state->SetTimingStartTick();
}

void GameUpdater::UpdatePlayer(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();

  // Store player state before moving
  AxisAlignedBox previous_player_box = player->GetBoundingBox();
  glm::vec3 previous_player_position = player->GetPosition();
  float previous_player_velocity = player->GetVerticalVelocity();

  // Check to see if the ground is no longer beneath the player,
  // in which case they should fall down
  if (player->GetGround()) {
    AxisAlignedBox ground_box = player->GetGround()->GetBoundingBox();
    // This will only consider when the player moves to the right of the
    // platform
    if (previous_player_box.GetMin().x > ground_box.GetMax().x) {
      // Ground is no longer beneath player
      player->RemoveGround();
    }
  }

  // determine new velocity, then position, then collision box
  if (player->GetGround() && player->GetSpacebarDown()) {
    // player should jump now
    player->SetVerticalVelocity(PLAYER_JUMP_VELOCITY);
    player->RemoveGround();
  } else if (player->GetGround()) {
    // player is stuck to ground, make sure velocity is neutralized
    player->SetVerticalVelocity(0.0f);
  } else {
    // player is in the air, apply gravity
    player->SetVerticalVelocity(previous_player_velocity - PLAYER_GRAVITY);
  }
  player->SetPosition(
      previous_player_position +
      glm::vec3(DELTA_X_PER_TICK, player->GetVerticalVelocity(), 0));
  AxisAlignedBox future_player_box = player->GetBoundingBox();

  std::vector<std::shared_ptr<Platform>> colliding_platforms =
      GetCollidingObjects(future_player_box,
                          game_state->GetLevel()->getPlatforms());

  std::vector<std::shared_ptr<Note>> colliding_collectibles =
      GetCollidingObjects(future_player_box,
                          game_state->GetLevel()->getNotes());

  for (int i = 0; i < colliding_collectibles.size(); i++) {
    if (!colliding_collectibles[i]->GetCollected()) {
      colliding_collectibles[i]->SetCollected();
      game_state->GetPlayer()->SetScore(game_state->GetPlayer()->GetScore() +
                                        1);
    }
  }

  if (colliding_platforms.size() > 1) {
    // TODO(jarhar): further consider what is happening when we are colliding
    // with two objects
    Reset(game_state);
  } else if (colliding_platforms.size() == 0) {
    // No collision
  } else {
    // Handle the collision
    std::shared_ptr<GameObject> colliding_object = colliding_platforms[0];
    // Are we in the air, and is the object we are colliding with below us?
    // If so, then set it as our current ground
    if (!player->GetGround()) {
      AxisAlignedBox colliding_box = colliding_object->GetBoundingBox();
      AxisAlignedBox player_box = player->GetBoundingBox();

      // If the player's bounding box was "above" the platform's box before
      // collision,
      // then we consider the collision "landing" on the platform
      float prev_player_min_y = previous_player_box.GetMin().y;
      float colliding_max_y = colliding_box.GetMax().y;
      if (prev_player_min_y > colliding_max_y) {
        // above colliding box, we are grounded on this platform
        // now attach player to the ground
        player->SetGround(colliding_object);
        player->SetVerticalVelocity(0);
        player->SetPosition(
            glm::vec3(player->GetPosition().x,
                      colliding_max_y + Player::PLATFORM_SPACING +
                          (player_box.GetMax().y - player_box.GetMin().y) / 2,
                      player->GetPosition().z));
      } else {
        // The collision was not from above, so reset the game
        Reset(game_state);
      }
    }
  }
}

// Returns the platform currently under the player
// TODO(jarhar): make this more efficient by calculating the current platform
//               based on music, like LevelUpdater::CurrentPlatform
std::shared_ptr<Platform> GetCurrentPlatform(std::shared_ptr<GameState> game_state) {
  // consider the rightmost part of the player to determine the current platform
  float player_x_location = game_state->GetPlayer()->GetBoundingBox().GetMax().x + 3.0f;
  std::shared_ptr<Platform> best_platform;
  for (std::shared_ptr<Platform> platform : *game_state->GetLevel()->getLevel()) {
    AxisAlignedBox platform_box = platform->GetBoundingBox();
    if (platform_box.GetMin().x < player_x_location && platform_box.GetMax().x > player_x_location) {
      // pick the rightmost platform to be the current platform
      if (!best_platform || best_platform->GetBoundingBox().GetMax().x < platform_box.GetMax().x) {
        best_platform = platform;
      }
    }
  }
  return best_platform;
}

void GameUpdater::UpdateCamera(std::shared_ptr<GameState> game_state) {

  std::shared_ptr<GameCamera> camera = game_state->GetCamera();

  glm::vec3 player_position = game_state->GetPlayer()->GetPosition();
  glm::vec3 previous_camera_position = camera->getPosition();
  glm::vec3 new_camera_position;

  std::shared_ptr<Platform> current_platform = GetCurrentPlatform(game_state);
  glm::vec3 current_platform_position;
  if (current_platform) {
    current_platform_position = current_platform->GetPosition();
  } else {
    // Fall back to player position if there is no platform
    current_platform_position = game_state->GetPlayer()->GetPosition();
  }
  float dY =
      (current_platform_position.y +
       DISTANCE_BELOW_CAMERA) -
      camera->getPosition()[1];

  float dX =
      (current_platform_position.x -
       DISTANCE_BEHIND_CAMERA) -
      camera->getPosition()[0];
  // smooth out camera transition
  if (std::abs(dY) > DELTA_Y) {
    if (dY < 0) {
      dY = -DELTA_Y;
    } else {
      dY = DELTA_Y;
    }
  }
  // always move camera forward
  if (dX < MIN_DELTA_X) {
    dX = MIN_DELTA_X;
  }


  // Z position is fixed
  new_camera_position.z = player_position.z + CAMERA_Z_SPACING;

  // Always keep camera aligned with player on x axis.
  // Make camera look ahead of the player
  new_camera_position.x = player_position.x + FORWARD_CAMERA_SPACING;

  // Gradually and smoothly move y towards player
  // float delta_y = player_position.y - previous_camera_position.y;
  new_camera_position.y =
      previous_camera_position.y + dY; // * FRACTION_CAMERA_MOVEMENT_PER_TICK;

  camera->setPosition(new_camera_position);

  // Always look directly at the player.
  // Add FORWARD_CAMERA_SPACING to align camera
  camera->setLookAt(glm::vec3(player_position.x + FORWARD_CAMERA_SPACING,
           camera->getLookAt().y + dY, 0));
}
