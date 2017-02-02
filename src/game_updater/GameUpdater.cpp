// Joseph Arhar

#include "game_updater/GameUpdater.h"

#include <glm/ext.hpp>
#include <iostream>

#include "helpers/Logging.h"
#include "helpers/TimingConstants.h"

namespace {

// TODO(jarhar): Figure out how to make this method signature more general purpose
std::vector<std::shared_ptr<GameObject>> GetCollidingObjects(
    AxisAlignedBox primary_object,
    std::shared_ptr<std::vector<std::shared_ptr<Platform>>> secondary_objects) {
  std::vector<std::shared_ptr<GameObject>> colliding_objects;
  // TODO(jarhar): make this more efficient by culling secondary objects
  for (std::shared_ptr<GameObject> secondary_object : *secondary_objects) {
    if (AxisAlignedBox::IsColliding(
          primary_object,
          secondary_object->GetBoundingBox())) {
      colliding_objects.push_back(secondary_object);
    }
  }

  return colliding_objects;
}

}

GameUpdater::GameUpdater()
    : level_updater(std::make_shared<LevelUpdater>()), done(false) {}

GameUpdater::~GameUpdater() {}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  level_updater->Update(game_state->GetLevel());

  UpdatePlayer(game_state);

  UpdateCamera(game_state);

  if (level_updater->Done()) {
    done = true;
    game_state->SetDone(done);
  }
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  level_updater->Reset(game_state->GetLevel());
  done = false;
  game_state->SetDone(done);
  game_state->GetPlayer()->SetPosition(Player::INITIAL_POSITION);
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
    // This will only consider when the player moves to the right of the platform
    if (previous_player_box.GetMin().x > ground_box.GetMax().x) {
      // Ground is no longer beneath player
      player->RemoveGround();
    }
  }

  // determine new velocity, then position, then collision box
  if (player->GetGround() && player->GetSpacebarDown()) {
    // player should jump now
    player->SetVerticalVelocity(Player::JUMP_VELOCITY);
    player->RemoveGround();
  } else if (player->GetGround()) {
    // player is stuck to ground, make sure velocity is neutralized
    player->SetVerticalVelocity(0.0f);
  } else {
    // player is in the air, apply gravity
    player->SetVerticalVelocity(previous_player_velocity - Player::GRAVITY);
  }
  player->SetPosition(
      previous_player_position +
      glm::vec3(DELTA_X_PER_TICK, player->GetVerticalVelocity(), 0));
  AxisAlignedBox future_player_box = player->GetBoundingBox();

  std::vector<std::shared_ptr<GameObject>> colliding_objects = GetCollidingObjects(
      future_player_box, game_state->GetLevel()->getLevel());

  if (colliding_objects.size() > 1) {
    // TODO(jarhar): further consider what is happening when we are colliding with two objects
    Reset(game_state);
  } else if (colliding_objects.size() == 0) {
    // No collision
  } else {
    // Handle the collision
    std::shared_ptr<GameObject> colliding_object = colliding_objects[0];
    // Are we in the air, and is the object we are colliding with below us?
    // If so, then set it as our current ground
    if (!player->GetGround()) {
      AxisAlignedBox colliding_box = colliding_object->GetBoundingBox();
      AxisAlignedBox player_box = player->GetBoundingBox();

      // If the player's bounding box was "above" the platform's box before collision,
      // then we consider the collision "landing" on the platform
      float prev_player_min_y = previous_player_box.GetMin().y;
      float colliding_max_y = colliding_box.GetMax().y;
      if (prev_player_min_y > colliding_max_y) {
        // above colliding box, we are grounded on this platform
        // now attach player to the ground
        player->SetGround(colliding_object);
        player->SetVerticalVelocity(0);
        player->SetPosition(glm::vec3(
          player->GetPosition().x,
          colliding_max_y + Player::PLATFORM_SPACING + (player_box.GetMax().y - player_box.GetMin().y) / 2,
          player->GetPosition().z));
      } else {
        // The collision was not from above, so reset the game
        Reset(game_state);
      }
    }
  }
}

void GameUpdater::UpdateCamera(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();

  glm::vec3 player_position = game_state->GetPlayer()->GetPosition();
  glm::vec3 previous_camera_position = camera->getPosition();
  glm::vec3 new_camera_position;

  // Z position is fixed
  new_camera_position.z = player_position.z + CAMERA_Z_SPACING;

  // Always keep camera aligned with player on x axis.
  // Make camera look ahead of the player
  new_camera_position.x = player_position.x + FORWARD_CAMERA_SPACING;

  // Gradually and smoothly move y towards player
  float delta_y = player_position.y - previous_camera_position.y;
  new_camera_position.y =
      previous_camera_position.y + delta_y * FRACTION_CAMERA_MOVEMENT_PER_TICK;

  camera->setPosition(new_camera_position);

  // Always look directly at the player.
  // Add FORWARD_CAMERA_SPACING to align camera
  camera->setLookAt(player_position + glm::vec3(FORWARD_CAMERA_SPACING, 0, 0));
}
